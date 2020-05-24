# Copyright (c) 2015 keen core
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.

require 'lace/remote_shell'

module Lace
	class RemoteBuilder
		def initialize( config, project_filename, target_platform, diagnostics_style )

			# find remote file mapping for the project path:
			remote_mapping = config.find_mapping( Pathname.new( project_filename ).expand_path.dirname, target_platform );

			if !remote_mapping
				puts "Could not find remote path mapping for directory #{Pathname.new( project_filename ).dirname}"
			end

			remote_host = remote_mapping.remote_host
			remote_user = remote_mapping.remote_user
			identity_file = remote_mapping.identity_file

			local_root = remote_mapping.local_path
			remote_root = remote_mapping.remote_path
			
			if !remote_root || !local_root
				puts "remote_root=#{remote_root}"
				puts "local_root=#{local_root}"
				raise 'Invalid remote build config'
			end

			@remote_host = remote_host
			@local_root = Pathname.new( local_root )
			@remote_root = remote_root
			@remote_shell = RemoteShell.new( config, remote_host, remote_user, identity_file )
			@sync_excludes = remote_mapping.exclude_pattern || []
			@remote_lace_bin_path = remote_mapping.remote_lace_path || map_path( Pathname.new( __FILE__ ).dirname + "../../bin" )
			@remote_lace = "#{@remote_lace_bin_path}/lace"
			@disable_rsync = remote_mapping.disable_rsync
			@lace_arguments = remote_mapping.lace_arguments
			@diagnostics_style = diagnostics_style
		end

		def local_path( p )
			return @remote_shell.encode_local_path "#{@local_root}/#{p}"
		end

		def remote_path( p )
			return "#{@remote_root}/#{p}"
		end

		def sync_source(project)
			unless @disable_rsync
				puts "sync source from local #{local_path('.')} to #{@remote_host}:#{remote_path('.')}"
				@remote_shell.rsync local_path( '.' ), remote_path( '.' ), @sync_excludes
			end
		end

		def remote_lace(cmd)
			#puts "running remote lace with command #{cmd}"
			remote_root = @remote_root.to_s
			local_root = @local_root.to_s
			@remote_shell.run_remote "ruby #{@remote_lace} #{cmd}" do |line|
				# map remote_root to local_root:
				puts line.gsub remote_root, local_root
			end
		end

		def remote_lace_xcode(local_project_path,xcode_proj_path)
			puts "running remote lace-xcode"
			@remote_shell.run_remote "ruby #{@remote_lace_bin_path}/lace-xcode -p #{map_path(local_project_path)}"

			if xcode_proj_path
				@remote_shell.run_remote "open #{map_path(xcode_proj_path)}"
			end
		end

		def map_path(p)
			rel = Pathname.new( p ).relative_path_from( @local_root )
			remote = Pathname.new( @remote_root ) + rel
			return remote.to_s
		end

		def clean(project)
			puts "clean build"
			remote_lace "#{@lace_arguments} -c -b #{project.build} #{map_path(project.filename)}"
		end

		def build(project)
			# todo: filter output for filenames and remap them:
			diagnostics_style_argument = @diagnostics_style == :msvc ? "--msvc-diagnostics" : ""
			remote_lace "#{@lace_arguments} -b #{project.build} #{diagnostics_style_argument} #{map_path(project.filename)}"
		end
	end
end


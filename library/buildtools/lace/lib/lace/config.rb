# This file implements the Lace::Context class.

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

require 'set'
require 'json'

module Lace

	class Config
		attr_reader :path, :blob_servers, :blob_local_path, :corefiler_lib, :corefiler_server_ip, :corefiler_server_port

		RemoteMapping = Struct.new :remote_host, :remote_user, :identity_file, :remote_path, :local_path, :exclude_pattern, :remote_lace_path, :disable_rsync, :lace_arguments
		BlobServer = Struct.new :type, :path, :directory, :user, :password

		def initialize( config_path = nil, default_blob_user = nil, default_blob_password = nil, default_corefiler_server = nil, default_corefiler_lib = nil )

			if config_path
				@path = config_path
				config = JSON.parse( config_path.read ) || {}
			else
				@path = "internal"
				default_config = {
					:blob_servers => {
						:core => {
							:type => "svn",
							:path => "svn://svn.keencore.com/blobs",
							:user => default_blob_user,
							:password => default_blob_password
						}
					}
				}.to_json
				config = JSON.parse( default_config )
			end
			remote_build_config = config['remote_build']

			if remote_build_config && remote_build_config['targets']
				@remote_target = remote_build_config['targets'][Helpers.user_name]
				if @remote_target
					@path_mappings = remote_build_config['path_mappings']
				else
					#puts "Warning: no remote target found for user #{Helpers.user_name} in config #{@path}"
					@path_mappings = nil
				end
			end

			@blob_servers = {}
			blob_servers_config = config[ 'blob_servers' ]

			if blob_servers_config
				blob_servers_config.each {|server_id,server_config|
					@blob_servers[server_id] = BlobServer.new(
						server_config[ 'type' ],
						Pathname.new( server_config[ 'path' ] ),
						server_config[ 'directory' ] || '',
						server_config[ 'user' ],
						server_config[ 'password'] )
				}
			end
			#puts "@blob_servers=#{@blob_servers}"

			config_blob_path = config[ 'blob_path' ]
			if config_blob_path
				@blob_local_path = Helpers.normalize_path( @path.dirname + config_blob_path )
			else
				@blob_local_path = Helpers.normalize_path( Pathname.new( ENV['LACE_BLOB_PATH'] || Helpers.default_blob_path ) )
			end

			@corefiler_lib 			= false
			@corefiler_server_ip 	= ''
			@corefiler_server_port 	= nil

			if default_corefiler_lib
				@corefiler_lib = true
			elsif config[ 'corefiler_lib' ]
				@corefiler_lib = config[ 'corefiler_lib' ]
			end

			if default_corefiler_server
				@corefiler_server_ip, @corefiler_server_port = default_corefiler_server.split( ':' )
			elsif config[ 'corefiler_server' ]
				@corefiler_server_ip, @corefiler_server_port = config[ 'corefiler_server' ].split( ':' )
			end

			@corefiler_server_port = 55550 if !@corefiler_server_port

			#puts "cflib #{@corefiler_lib}  cfserver #{corefiler_server_ip}:#{corefiler_server_port}"
		end

		def find_mapping(dir,target_platform)
			remote_target = @remote_target.select { |target_key,target_value| target_key.split(',').include?( target_platform.to_s ) }.first
			raise "No remote target found for target platform #{target_platform} - check your lace-config.json!" if !remote_target
			remote_target = remote_target[1]
			@path_mappings.each { |mapping|
				local_mapping_path = @path.dirname + mapping['local_path']
				#p "Testing path #{local_mapping_path}"
				if Helpers.is_child_dir_of( dir, local_mapping_path )
					remote_mapping_path = Pathname.new( remote_target['path'] ) + mapping['remote_path']
					return RemoteMapping.new(remote_target['host'], remote_target['user'], remote_target['identity'], remote_mapping_path, local_mapping_path, mapping['exclude_pattern'], mapping['remote_lace_path'], remote_target['disable_rsync'], remote_target['lace_arguments'])
				end
			}
			return nil
		end

		def self.load(path,default_blob_user=nil,default_blob_password=nil,default_corefiler_server=nil,default_corefiler_lib=nil)
			# search for the config:
			while !File.exists?( path + "lace_config.json" )
				new_path = path.dirname

				if new_path == path
					# no config found: create default config:
					return self.new nil, default_blob_user, default_blob_password, default_corefiler_server, default_corefiler_lib
				end
				path = new_path
			end

			config_path = path + "lace_config.json"
			self.new( config_path, default_blob_user, default_blob_password, default_corefiler_server, default_corefiler_lib )
		end

	end
end


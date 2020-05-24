require 'pathname'

def getRepositoryTypeAndPath( path )

	repPath = Pathname.new( path )
	lastpath = repPath

	gitPath = ''
	svnPath = ''

	begin

		if ( repPath + '.svn' ).exist?
			svnPath = repPath.to_s
		end

		if ( repPath + '.git' ).exist?
			gitPath = repPath.to_s
		end

		lastpath = repPath
		repPath = repPath.parent()

	end while( repPath != lastpath ) && ( svnPath.empty? )

	return 'git', gitPath if !gitPath.empty?
	return 'svn', svnPath if !svnPath.empty?
	return '', ''
end

def writeFile( path, content )
	if !File.exist?( path ) || File.read( path ) != content
		puts "Generating #{File.basename( path )}"
		File.write( path, content )
	end
end

projectNameString		= ARGV[0]
project_base_path		= Pathname.new( ARGV[1] ).realpath
cpp_name				= ARGV[2]
projectBuildPath		= ARGV[3]
gamebuildDirectory		= ARGV[4]
configuration			= ARGV[5]
release_version			= ARGV[6]
projectShaderDirectory	= Pathname.new(projectBuildPath + '/shader')
build_version			= 'No version'
buildBranchName			= ''

projectShaderDirectory = projectShaderDirectory.expand_path

is_jenkins_build = ENV['LACE_BUILD_VERSION']

if is_jenkins_build
	#puts "Jenkins build detected.. using dummy version.cpp"
	build_version = "Jenkins-#{ENV['LACE_BUILD_VERSION']}"
	buildBranchName = ""
else
#if configuration != "debug"

	repType, repPath = getRepositoryTypeAndPath( project_base_path )

	#p "##### #{repType} #{repPath}"

	if( repType == 'svn' )
		if ENV['SVN_PATH'] != nil
			p = Pathname.new( ENV['SVN_PATH'].gsub(/^"(.*?)"$/,'\1').gsub /\\+/, '/' )
			svn_info_filename = p.join( 'bin/svn.exe' )
		else
			svn_info_filename = 'svn'
		end

		begin
			#p "#{svn_info_filename} info #{repPath}"
			svn_info_content = `"#{svn_info_filename}" info "#{repPath}"`
			svn_info_content.split( /\n/ ).each do |line|
				if line.start_with?( 'URL: ' )
					line.split( /\// ).each do |name|
						buildBranchName = name
					end
				elsif line.start_with?( 'Revision: ' )
					build_version = line.split(':')[1].strip
				elsif line.start_with?( 'Last Changed Rev: ' )
					build_version = line.split(':')[1].strip
				end
			end
		rescue
		end

	elsif( repType == 'git' )

		current_dir = Dir.getwd
		begin
			Dir.chdir( repPath )
			build_version = `git rev-parse HEAD`.strip
			if build_version =~ /\-(\d+)($|\-)/
				build_version = $1
			end
			Dir.chdir(current_dir)
			buildBranchName = `git rev-parse --abbrev-ref HEAD`.chomp
		rescue
			build_version = 'No repository version found'
			buildBranchName = ''
		end
		Dir.chdir(current_dir)
	else
		build_version = 'No repository version found'
		buildBranchName = ''
	end

end

cpp = "namespace keen\n{\n"
cpp << sprintf("\tconst char* getBuildVersionString()\n\t{\n\t\treturn \"%s\";\n\t}\n\n", build_version)
cpp << sprintf("\tconst char* getReleaseVersionString()\n\t{\n\t\treturn \"%s\";\n\t}\n\n", release_version)
cpp << sprintf("\tconst char* getBuildConfigurationString()\n\t{\n\t\treturn \"%s\";\n\t}\n\n", configuration)
cpp << sprintf("\tconst char* getBuildProjectName()\n\t{\n\t\treturn \"%s\";\n\t}\n\n", projectNameString)
cpp << sprintf("\tconst char* getBuildBranchNameString()\n\t{\n\t\treturn \"%s\";\n\t}\n\n", buildBranchName)
cpp << sprintf("\tconst char* getProjectShaderDirectoryString()\n\t{\n\t\treturn \"%s\";\n\t}\n\n", projectShaderDirectory)
cpp << sprintf("\tconst char* getGamebuildDirectoryString()\n\t{\n\t\treturn \"%s\";\n\t}\n\n", gamebuildDirectory)
cpp << sprintf("\tconst char* getProjectBaseDirectoryString()\n\t{\n\t\treturn \"%s\";\n\t}\n\n", project_base_path)
cpp << "}\n"

hpp = "namespace keen\n{\n"
hpp << "\tconst char* getBuildVersionString();\n"
hpp << "\tconst char* getReleaseVersionString();\n"
hpp << "\tconst char* getBuildConfigurationString();\n"
hpp << "\tconst char* getBuildProjectName();\n"
hpp << "\tconst char* getBuildBranchNameString();\n"
hpp << "\tconst char* getProjectShaderDirectoryString();\n"
hpp << "\tconst char* getGamebuildDirectoryString();\n"
hpp << "\tconst char* getProjectBaseDirectoryString();\n"
hpp << "}\n"

writeFile( cpp_name, cpp )
writeFile( cpp_name.chomp( '.cpp' ) + '.hpp', hpp )
writeFile( Pathname.new( projectBuildPath ) + 'build_branch.txt', "#{buildBranchName}" )
writeFile( Pathname.new( projectBuildPath ) + 'build_version.txt', "#{build_version}" )
writeFile( Pathname.new( projectBuildPath ) + 'release_version.txt', "#{release_version}" )


# detect host and target platforms:
BuildInfo ||= Struct.new(:id, :platform, :cpu, :endianness, :pointer_size, :executable_extension, :static_library_extension )

# keep this table in sync with platform.hpp/platform.cpp:
$buildInfos = {
	'ngc'			=> BuildInfo.new( :ngc,				:ngc,		:ppc32,		:big,		4, '', '.a' ),
	'wii'			=> BuildInfo.new( :wii,				:wii,		:ppc32,		:big,		4, '', '.a' ),
	'wiiu'			=> BuildInfo.new( :wiiu,			:wiiu,		:ppc32,		:big,		4, '', '.a' ),
	'nds'			=> BuildInfo.new( :nds,				:nds,		:arm32,		:little,	4, '', '.a' ),
	'ctr'			=> BuildInfo.new( :ctr,				:ctr,		:arm32,		:little,	4, '', '.a' ),
	'ps2'			=> BuildInfo.new( :ps2,				:ps2,		:mips32,	:little,	4, '', '.a' ),
	'ps3'			=> BuildInfo.new( :ps3,				:ps3,		:ppc32,		:big,		4, '', '.a' ),
	'ps4'			=> BuildInfo.new( :ps4,				:ps4,		:x64,		:little,	8, '', '.a' ),
	'psp'			=> BuildInfo.new( :psp,				:psp,		:mips32,	:little,	4, '', '.a' ),
	'psvita'		=> BuildInfo.new( :psvita,			:psvita,	:arm32,		:little,	4, '', '.a' ),
	'xb360'			=> BuildInfo.new( :xb360,			:xb360,		:ppc32,		:big,		4, '', '.lib' ),
	'xb1'			=> BuildInfo.new( :xb1,				:xb1,		:x64,		:little,	8, '.exe', '.lib' ),
	'nx'			=> BuildInfo.new( :nx,				:nx,		:arm64,		:little,	8, '', '.a' ),
	'nx_win32'		=> BuildInfo.new( :nx_win32,		:nx,		:x64,		:little,	8, '.exe', '.a' ),
	'win32_x86'		=> BuildInfo.new( :win32_x86,		:win32,		:x86,		:little,	4, '.exe', '.lib' ),
	'win32_x64'		=> BuildInfo.new( :win32_x64,		:win32,		:x64,		:little,	8, '.exe', '.lib' ),
	'linux_x86'		=> BuildInfo.new( :linux_x86,		:linux,		:x86,		:little,	4, '', '.a' ),
	'linux_x64'		=> BuildInfo.new( :linux_x64,		:linux,		:x64,		:little,	8, '', '.a' ),
	'osx_x86'		=> BuildInfo.new( :osx_x86,			:osx,		:x86,		:little,	4, '', '.a' ),
	'osx_x64'		=> BuildInfo.new( :osx_x64,			:osx,		:x64,		:little,	8, '', '.a' ),
	'ios_arm32'		=> BuildInfo.new( :ios_arm32,		:ios,		:arm32,		:little,	4, '.app', '.a' ),
	'ios_arm64'		=> BuildInfo.new( :ios_arm64,		:ios,		:arm64,		:little,	8, '.app', '.a' ),
	'android_arm32' => BuildInfo.new( :android_arm32,	:android,	:arm32,		:little,	4, '.apk', '.a' ),
	'android_arm64' => BuildInfo.new( :android_arm64,	:android,	:arm64,		:little,	8, '.apk', '.a' ),
	'uwp_x86'		=> BuildInfo.new( :uwp_x86,			:uwp,		:x86,		:little,	4, '.exe', '.lib' ),
	'uwp_x64'		=> BuildInfo.new( :uwp_x64,			:uwp,		:x64,		:little,	8, '.exe', '.lib' ),
	'uwp_arm32'		=> BuildInfo.new( :uwp_arm32,		:uwp,		:arm32,		:little,	4, '.exe', '.lib' ),
	'uwp_arm64'		=> BuildInfo.new( :uwp_arm64,		:uwp,		:arm64,		:little,	8, '.exe', '.lib' ),
}

$shared_platform_map = {
	:linux => [ :posix ],
	:ios => [ :posix ],
	:android => [ :posix ],
	:osx => [ :posix ],
	:win32 => [ :windows ],
	:uwp => [ :windows ],
	:xb1 => [ :windows ],
}

def get_implicit_platform_tags( platform )
	tags = []
	tags << :posix if [:linux,:ios,:osx,:android].include?( platform )
	tags << :windows if [:win32,:uwp,:xb1].include?( platform )
	tags << :nvn if [:nx].include?( platform )
	tags << :np if [:ps3,:ps4].include?( platform )
	tags << :gnm if [:ps4].include?( platform )
	return tags
end

def get_platform_tags( platforms )
	tags = []
	platforms.each { |platform|
		tags << platform
		tags += get_implicit_platform_tags( platform )
	}
	return tags.uniq
end

def get_all_platform_tags()
	all_platforms = []
	$buildInfos.each { |build,info|
		all_platforms << info.platform
	}
	all_platforms.uniq!
	return get_platform_tags( all_platforms )
end

def get_disabled_platform_tags(platforms)
	all_tags = get_all_platform_tags
	active_tags = get_platform_tags( platforms )
	return all_tags - active_tags
end
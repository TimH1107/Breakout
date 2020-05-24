require 'socket'

host_ip = Socket.ip_address_list.select{ |ip| ip.ipv4? && !ip.ipv4_loopback? }.first

cpp = ""
cpp << "namespace keen\n{\n"
cpp << "\tconst char* getHostIpString()\n\t{\n\t\treturn \"#{host_ip.ip_address}\";\n\t}\n"
cpp << "}\n"

hpp = ""
hpp << "namespace keen\n{\n"
hpp << "\tconst char* getHostIpString();\n"
hpp << "}\n"

cpp_name = ARGV[0]
if !File.exist?( cpp_name ) || File.read( cpp_name ) != cpp
	puts 'Generating ' + File.basename( cpp_name )
	File.write( cpp_name, cpp )
end

hpp_name = cpp_name.chomp( '.cpp' ) + '.hpp'
if !File.exist?(hpp_name) || File.read( hpp_name ) != hpp
	puts 'Generating ' + File.basename( hpp_name )
	File.write( hpp_name, hpp )
end

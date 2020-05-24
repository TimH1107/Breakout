require 'zlib'
require 'fileutils'

def gen_crc(infile, outfile, mixed_case)
	file_def = '_' + File.basename(outfile).upcase.gsub(/\.| /, '_')

	FileUtils.mkpath(File.dirname(outfile))
	File.open(outfile, 'w') do |file|
		file.printf "#ifndef %s\n#define %s\n\n", file_def, file_def

		File.readlines(infile).each do |line|
			m = /^([^#]*)(#.*)?$/.match(line.strip)
			raise "malformed line: '#{line.strip}'" unless m
			id = m[1].strip
			raise "Illegal crc id: #{id.inspect} in source file #{infile}" if id =~ /[^\w \.]/
			unless id.empty?
				id = id.downcase unless mixed_case
				crc = Zlib.crc32(id)
				define = id.upcase.gsub(/[^A-Z0-9]/, '_')
				file.printf "#define CRC_%s 0x%08xU", define, crc
				file.printf "\t" if m[2]
			end
			file.printf "//%s", m[2][1..-1] if m[2]
			file.puts
		end

		file.printf "\n#endif // %s\n", file_def
	end
end

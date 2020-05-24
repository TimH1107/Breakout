require 'fileutils'	
	
def gen_bin2c(infile, outfile)
	file_def = 'KEEN_' + File.basename(outfile).upcase.gsub(/[\. -]/, '_')	
	var_name = 's_' + File.basename(infile).downcase.gsub(/[\. -]/, '_')

	FileUtils.mkpath(File.dirname(outfile))

	File.open(outfile, 'w') do |file|
		file.printf "#ifndef %s\n#define %s\n\n", file_def, file_def
		file.printf "#include \"keen/base/types.hpp\"\n\n"
		file.printf "namespace keen\n{\n"
		file.printf "\tnamespace BinaryData\n\t{\n"
		file.printf "\t\tKEEN_ALIGN_PREFIX( 16 ) static const uint8 %s[] KEEN_ALIGN_POSTFIX( 16 ) =\n\t\t{", var_name

		File.open(infile, 'rb') do |ifile|
			data = ifile.read()
			count = 0
			data.each_byte do |value|
				file.printf "\n" if count & 63 == 0
				file.printf "%d,", value
				count += 1
			end
		end	

		file.printf "\n\t\t};\n"
		file.printf "\t}\n}\n\n#endif // %s\n", file_def
	end
end

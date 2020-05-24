
module Lace

	class Profiler

		def self.start()
			@@chrome_trace = { "traceEvents" => [] }
			@@start_time = Process.clock_gettime(Process::CLOCK_MONOTONIC)
		end

		def self.write_result(path)
			append_trace_section @@start_time, Process.clock_gettime(Process::CLOCK_MONOTONIC), 0, "Lace"
			File.write( path, @@chrome_trace.to_json )
		end

		@m = Mutex.new

		def self.append_trace_section( t0, t1, tid, zone, name = nil )
			return unless defined? @@chrome_trace

			time_us = ( ( t0 - @@start_time ) * 1000000.0 ).to_i
			dur_us = ( ( t1 - t0 ) * 1000000.0 ).to_i

			#p "append zone:#{zone} tid:#{tid} name:#{name}"

			@m.synchronize {
				@@chrome_trace[ "traceEvents" ] << {
					"pid" => 0,
					"tid" => tid,
					"ph" => "X",
					"ts" => time_us,
					"dur" => dur_us,
					"name" => zone + ( name ? ": #{name}" : "" ),
					"args" => { "zone" => name ? zone : "" }
				}
			}
		end

		def self.section(zone,name = nil)
			return unless defined? @@chrome_trace
			tid = Thread.current[ :profiler_tid ] || 0
			startTime = Process.clock_gettime(Process::CLOCK_MONOTONIC)
			value = yield
			endTime = Process.clock_gettime(Process::CLOCK_MONOTONIC)
			append_trace_section startTime, endTime, tid, zone, name
			value
		end

		def self.profile_method(klass, method_name, name_mapping_block = "" )
			return unless defined? @@chrome_trace
			patch = <<-RUBY
				unless defined?(#{method_name}__mp_unpatched)
					alias_method :#{method_name}__mp_unpatched, :#{method_name}
					def #{method_name}(*args, &blk)
					    start_time = Process.clock_gettime(Process::CLOCK_MONOTONIC)
					    result = #{method_name}__mp_unpatched(*args, &blk)
					  	end_time = Process.clock_gettime(Process::CLOCK_MONOTONIC)
					  	tid = Thread.current[ :profiler_tid ] || 0
					  	section_name = #{name_mapping_block}
					    Lace::Profiler::append_trace_section( start_time, end_time, tid, \"#{method_name}\", section_name )
					    return result
					end
				end
				RUBY

			#puts patch
			klass.class_eval patch
		end
	end
end

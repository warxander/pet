#pragma once

#include <toolkit/Macro.hpp>

#include <chrono>
#include <iostream>

namespace pet
{
	class Profiler
	{
	private:
		std::string													_message;
		std::chrono::time_point<std::chrono::high_resolution_clock> _time;
		int64_t														_threshold;

	public:
		explicit Profiler(const std::string& message, int64_t threshold = 0)
			: _message(message), _time(std::chrono::high_resolution_clock::now()), _threshold(threshold)
		{
		}

		~Profiler()
		{
			PET_TRY("Can't destroy profiler", {
				const auto now = std::chrono::high_resolution_clock::now();
				const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>((now - _time)).count();
				if (duration > _threshold)
					std::cout << StringBuilder() % duration % " milliseconds - " % _message << std::endl;
			});
		}
	};

#ifdef PET_ENABLE_PROFILING
#define PET_PROFILE_DEBUG(Message) const Profiler profiler(Message)
#define PET_PROFILE_DEBUG_THRESHOLD(Message, Threshold) const Profiler profiler(Message, Threshold)
#else
#define PET_PROFILE_DEBUG(...)
#define PET_PROFILE_DEBUG_THRESHOLD(...)
#endif
}

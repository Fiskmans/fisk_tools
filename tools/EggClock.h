#ifndef FISK_TOOLS_EGG_CLOCK_H
#define FISK_TOOLS_EGG_CLOCK_H

#include <chrono>

namespace fisk::tools
{
	class EggClock
	{
	public:
		EggClock(std::chrono::milliseconds aDuration);

		bool IsDone();

	private:

		std::chrono::milliseconds myDuration;
		std::chrono::time_point<std::chrono::steady_clock> myStart;
	};
}

#endif
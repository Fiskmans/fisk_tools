#include "tools/EggClock.h"


namespace fisk::tools
{
	EggClock::EggClock(std::chrono::milliseconds aDuration)
		: myDuration(aDuration)
	{
		myStart = std::chrono::steady_clock::now();
	}

	bool EggClock::IsDone()
	{
		std::chrono::time_point now = std::chrono::steady_clock::now();

		std::chrono::milliseconds dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - myStart);

		return dt > myDuration;
	}
}
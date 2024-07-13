
#include "tools/PlatformDetection.h"

#if FISK_PLATFORM_LINUX

#include "tools/Socket.h"

#include <unistd.h>

namespace fisk::tools
{
	Socket::~Socket()
	{
		if (myValue != -1)
			::close(myValue);
	}
} // namespace fisk::tools

#endif
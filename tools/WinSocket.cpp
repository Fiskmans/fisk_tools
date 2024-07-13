
#include "tools/PlatformDetection.h"

#if FISK_PLATFORM_WINDOWS

#include "tools/Socket.h"

namespace fisk::tools
{
	Socket::~Socket()
	{
		if (myValue != INVALID_SOCKET)
			::closesocket(myValue);
	}
} // namespace fisk::tools

#endif
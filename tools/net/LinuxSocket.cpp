
#include "tools/PlatformDetection.h"

#if FISK_PLATFORM_LINUX

#include "tools/net/Socket.h"

#include <unistd.h>

namespace fisk::tools
{
	Socket::~Socket()
	{
		if (myValue != -1)
			::close(myValue);
	}

	std::string Socket::GetAddress() const
	{
		return "<not yet implemented>";
	}

} // namespace fisk::tools

#endif

#include "tools/PlatformDetection.h"

#if FISK_PLATFORM_WINDOWS

#include "tools/Net.h"

#include "tools/Logger.h"

#include <WinSock2.h>
#define NOMINMAX
#include <Windows.h>

namespace fisk::tools
{
	void Net::StartUp()
	{
		static bool initialized = false;

		if (initialized)
			return;

		WSADATA data;
		int err = ::WSAStartup(MAKEWORD(2, 2), &data);

		if (err != 0)
			LOG_SYS_ERROR("Failed to start network system", "Error: " + std::to_string(err));

		initialized = true;

		return;
	}

	void Net::Shutdown()
	{
	}
} // namespace fisk::tools
#endif
#ifndef FISK_TOOLS_SOCKET_H
#define FISK_TOOLS_SOCKET_H

#include "tools/PlatformDetection.h"

#include "tools/Net.h"

#if FISK_PLATFORM_WINDOWS

#define NOMINMAX
#include <WinSock2.h>
#include <Windows.h>

#endif

#include <cstdint>

namespace fisk::tools
{
	using Port = uint16_t;

	class Socket 
	{
	public:
		Socket() = default;
		~Socket();

		Socket(const Socket& aOther)		 = delete;
		void operator=(const Socket& aOther) = delete;

		NetHandle myHandle = Net::GetInstance().Use();

#if FISK_PLATFORM_WINDOWS
		SOCKET myValue = INVALID_SOCKET;
#else
		int myValue = -1;
#endif
	};
}

#endif

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

	uint16_t GetPort(sockaddr* aSockaddr)
	{
		if (aSockaddr->sa_family == AF_INET)
			return ntohs(reinterpret_cast<sockaddr_in*>(aSockaddr)->sin_port);

		return ntohs(reinterpret_cast<sockaddr_in6*>(aSockaddr)->sin6_port);
	}

	std::string Socket::GetAddress() const
	{
		if (myValue == INVALID_SOCKET)
			return "<invalid>";

		sockaddr_in6 addr;

		memset(&addr, 0, sizeof(addr));

		int length = sizeof(addr);

		if (::getpeername(myValue, reinterpret_cast<sockaddr*>(&addr), &length) == SOCKET_ERROR)
			return "<error>";

		char s[INET6_ADDRSTRLEN];

		sockaddr* base = reinterpret_cast<sockaddr*>(&addr);

		char stringBuffer[INET6_ADDRSTRLEN];
		const char* formattedString = inet_ntop(base->sa_family, GetAddressStart(base), stringBuffer, sizeof(stringBuffer));

		if (!formattedString)
			return "<formatting failed>";

		std::string out = formattedString;

		return out + ":" + std::to_string(GetPort(base));
	}

} // namespace fisk::tools

#endif

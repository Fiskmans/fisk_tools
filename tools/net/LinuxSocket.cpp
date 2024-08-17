
#include "tools/PlatformDetection.h"

#if FISK_PLATFORM_LINUX

#include "tools/net/Socket.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>

namespace fisk::tools
{
	Socket::~Socket()
	{
		if (myValue != -1)
			::close(myValue);
	}

	void* GetAddressStart(sockaddr* aSockaddr)
	{
		if (aSockaddr->sa_family == AF_INET)
			return &(reinterpret_cast<sockaddr_in*>(aSockaddr)->sin_addr);

		return &(reinterpret_cast<sockaddr_in6*>(aSockaddr)->sin6_addr);
	}

	uint16_t GetPort(sockaddr* aSockaddr)
	{
		if (aSockaddr->sa_family == AF_INET)
			return ntohs(reinterpret_cast<sockaddr_in*>(aSockaddr)->sin_port);

		return ntohs(reinterpret_cast<sockaddr_in6*>(aSockaddr)->sin6_port);
	}

	std::string Socket::GetAddress() const
	{
		if (myValue == -1)
			return "<invalid>";

		sockaddr_in6 addr;

		memset(&addr, 0, sizeof(addr));

		socklen_t length = sizeof(addr);

		if (::getpeername(myValue, reinterpret_cast<sockaddr*>(&addr), &length) == -1)
			return "<error>";

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

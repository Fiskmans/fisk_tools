#pragma once

#include "tools/Stream.h"

namespace fisk::tools::http
{
	class IConnection
	{
	public:
		virtual ~IConnection() = default;

		virtual ReadStream& GetReadStream() = 0;
		virtual WriteStream& GetWriteStream() = 0;
	};
}
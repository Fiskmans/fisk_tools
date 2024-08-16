#pragma once

#include "tools/Stream.h"
#include "tools/http/Frame.h"

namespace fisk::tools::http
{
	class IConnection
	{
	public:
		virtual ~IConnection() = default;

		virtual ReadStream& GetReadStream() = 0;
		virtual WriteStream& GetWriteStream() = 0;


		enum class RequestResult
		{
			Use_Default,
			Has_Responded,
			Should_Terminate
		};

		RequestResult Send(ResponseFrame& aFrame);
	};
}
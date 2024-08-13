#pragma once

#include "tools/Event.h"

#include "tools/http/EndPoint.h"

namespace fisk::tools::http
{
	class Server
	{
	public:

		Server();

		enum class FilterMode
		{
			Start,
			Full
		};

		void AddEndpoint(RequestFrame::Method aMethodFilter, FilterMode aPathFilterMode, std::string aPathFilter, Endpoint* aEndpoint);

		void UpdateOn(ReadStream& aReadStream, WriteStream& aWriteStream);

		void SetDefaultResponse(ResponseFrame aResponse);
	private:

		class EndpointMapping
		{
		public:
			EndpointMapping(RequestFrame::Method aMethodFilter, FilterMode aPathFilterMode, std::string aPathFilter, Endpoint* aEndpoint, ShortCircutableEvent<ResponseFrame, RequestFrame*>& aEvent);

		private:

			std::optional<ResponseFrame> OnFrame(RequestFrame* aFrame);

			EventReg myEventHandle;


			RequestFrame::Method myMethod;
			FilterMode myPathFilterMode;
			std::string myPathFilter;
			Endpoint* myEndpoint;
		};

		std::vector<std::unique_ptr<EndpointMapping>> myMappings;

		ResponseFrame myDefaultResponse;

		ShortCircutableEvent<ResponseFrame, RequestFrame*> OnFrame;
	};
}
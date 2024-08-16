#pragma once

#include "tools/Event.h"
#include "tools/Stream.h"

#include "tools/http/EndPoint.h"
#include "tools/http/IConnection.h"

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

		void UpdateOn(IConnection& aConnection);

		void SetDefaultResponse(ResponseFrame aResponse);
	private:

		class EndpointMapping
		{
		public:
			EndpointMapping(RequestFrame::Method aMethodFilter, FilterMode aPathFilterMode, std::string aPathFilter, Endpoint* aEndpoint, ShortCircutableEvent<IConnection::RequestResult, RequestFrame*, IConnection*>& aEvent);

		private:

			std::optional<IConnection::RequestResult> OnFrame(RequestFrame* aFrame, IConnection* aConnection);

			EventReg myEventHandle;


			RequestFrame::Method myMethod;
			FilterMode myPathFilterMode;
			std::string myPathFilter;
			Endpoint* myEndpoint;
		};

		std::vector<std::unique_ptr<EndpointMapping>> myMappings;

		ResponseFrame myDefaultResponse;

		ShortCircutableEvent<IConnection::RequestResult, RequestFrame*, IConnection*> OnFrame;
	};
}
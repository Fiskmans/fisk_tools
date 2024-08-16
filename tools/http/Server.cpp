#include "tools/http/Server.h"

namespace fisk::tools::http
{
	Server::Server()
	{
		myDefaultResponse.myCode = CommonResponseCodes::NotFound;

		myDefaultResponse.WriteRawHtml("<h1>The requested endpoint could not be found<h1>");
		myDefaultResponse.CalculateSize();
	}

	void Server::AddEndpoint(RequestFrame::Method aMethodFilter, FilterMode aPathFilterMode, std::string aPathFilter, Endpoint* aEndpoint)
	{
		myMappings.push_back(std::make_unique<EndpointMapping>(aMethodFilter, aPathFilterMode, aPathFilter, aEndpoint, OnFrame));
	}

	void Server::UpdateOn(IConnection& aConnection)
	{
		while (true)
		{
			std::optional<RequestFrame> frame = RequestFrame::FromStream(aConnection.GetReadStream());

			if (!frame)
				break;

			std::optional<IConnection::RequestResult> result = OnFrame.Fire(&(*frame), &aConnection);

			if (result)
			{
				switch (*result)
				{
					
				case IConnection::RequestResult::Use_Default:
					aConnection.Send(myDefaultResponse);
					break;
				case IConnection::RequestResult::Has_Responded:
					break;
				case IConnection::RequestResult::Should_Terminate:
					return;
				}
			}
			else
			{
				aConnection.Send(myDefaultResponse);
			}
		}
	}

	void Server::SetDefaultResponse(ResponseFrame aResponse)
	{
		myDefaultResponse = aResponse;
	}

	Server::EndpointMapping::EndpointMapping(RequestFrame::Method aMethodFilter, FilterMode aPathFilterMode, std::string aPathFilter, Endpoint* aEndpoint, ShortCircutableEvent<IConnection::RequestResult, RequestFrame*, IConnection*>& aEvent)
		: myMethod(aMethodFilter)
		, myPathFilterMode(aPathFilterMode)
		, myPathFilter(aPathFilter)
		, myEndpoint(aEndpoint)
	{
		myEventHandle = aEvent.Register(std::bind(&EndpointMapping::OnFrame, this, std::placeholders::_1, std::placeholders::_2));
	}

	std::optional<IConnection::RequestResult> Server::EndpointMapping::OnFrame(RequestFrame* aFrame, IConnection* aConnection)
	{
		if (!(aFrame->myMethod & myMethod))
			return {};

		switch (myPathFilterMode)
		{
		case fisk::tools::http::Server::FilterMode::Start:
			if (aFrame->myPath.starts_with(myPathFilter))
				return {};
		case fisk::tools::http::Server::FilterMode::Full:
			if (aFrame->myPath != myPathFilter)
				return {};
		}

		return myEndpoint->OnFrame(*aFrame, *aConnection);
	}
}

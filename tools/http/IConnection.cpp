#include "IConnection.h"

namespace fisk::tools::http
{
    IConnection::RequestResult IConnection::Send(ResponseFrame& aFrame)
    {
        aFrame.ToStream(GetWriteStream());
        return RequestResult::Has_Responded;
    }
}

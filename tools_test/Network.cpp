

#include <catch2/catch_all.hpp>

#include "tools/EggClock.h"
#include "tools/net/TCPListenSocket.h"
#include "tools/net/TCPSocket.h"

#include "tools/SystemValues.h"

TEST_CASE("Listen socket", "[Network]")
{
	fisk::tools::TCPListenSocket listenSocket(fisk::tools::TCPListenSocket::AnyPort);

	std::shared_ptr<fisk::tools::TCPSocket> connectedSocket;

	std::unique_ptr<fisk::tools::EventRegistration> reg = listenSocket.OnNewConnection.Register(
		[&connectedSocket](std::shared_ptr<fisk::tools::TCPSocket> aSocket) { connectedSocket = aSocket; });

	using namespace std::chrono_literals;

	fisk::tools::TCPSocket clientSocket("localhost", std::to_string(listenSocket.GetPort()).c_str(), 500ms);

	{
		fisk::tools::EggClock timer(500ms);

		bool update = true;
		while (!connectedSocket)
		{
			update &= listenSocket.Update();

			if (timer.IsDone())
				break;

			if (!update)
				break;
		}

		REQUIRE(update);
		REQUIRE(!timer.IsDone());
		REQUIRE(connectedSocket);
	}

	const uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8};
	{
		clientSocket.GetWriteStream().WriteData(data, sizeof(data));
	}

	{
		fisk::tools::EggClock timer(500ms);
		bool client	   = true;
		bool connected = true;

		bool hasRead = false;
		uint8_t read[sizeof(data)];

		std::unique_ptr<fisk::tools::EventRegistration> dataReg =
			connectedSocket->OnDataAvailable.Register([&hasRead, &connectedSocket, &read]() {
				fisk::tools::ReadStream& stream = connectedSocket->GetReadStream();

				if (!stream.Read(read, sizeof(read)))
				{
					stream.RestoreRead();
					return;
				}

				stream.CommitRead();
				hasRead = true;
			});

		while (!hasRead)
		{
			client &= clientSocket.Update();
			connected &= connectedSocket->Update();

			if (timer.IsDone())
				break;

			if (!client)
				break;
			if (!connected)
				break;
		}

		REQUIRE(!timer.IsDone());
		REQUIRE(memcmp(data, read, sizeof(data)) == 0);

		REQUIRE(client);
		REQUIRE(connected);
	}
}

TEST_CASE("SystemValues", "[network]")
{
	fisk::tools::SystemValues mockLocal;
	fisk::tools::SystemValues mockRemote;

	fisk::tools::SystemValues::Difference diff = mockLocal.Differences(mockRemote);

	REQUIRE(!diff);
	REQUIRE(!diff.Contains("float"));
	REQUIRE(!diff.Contains("float.digits"));
	REQUIRE(!diff.Contains("int"));
	REQUIRE(!diff.Contains("int.digits"));

	{
		mockLocal.myFloatValues.myDigits = 10;
	}

	{
		mockRemote.myFloatValues.myDigits = 100;
	}

	diff = mockLocal.Differences(mockRemote);

	REQUIRE(diff);
	REQUIRE(diff.Contains("float"));
	REQUIRE(diff.Contains("float.digits"));
	REQUIRE(!diff.Contains("int"));
	REQUIRE(!diff.Contains("int.digits"));
}
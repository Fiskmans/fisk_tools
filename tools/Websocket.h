#pragma once 

#include "tools/Event.h"
#include "tools/Stream.h"

#include <vector>

namespace fisk::tools
{
	class Websocket
	{
	public:
		Websocket(ReadStream& aReadStream, WriteStream& aWriteStream);

		bool Update();

		struct Blob
		{
			enum class Type
			{
				Binary,
				Text
			};

			Type myType;
			const std::vector<uint8_t>* myData;
		};

		Event<Blob> OnData;

		void SendData();
		void SendText(std::string aData);

		void Close();

	private:

		struct Frame
		{
			enum Opcode : uint8_t
			{
				Continuation = 0,
				Text = 1,
				Binary = 2,

				Close = 8,
				Ping = 9,
				Pong = 10,

				INVALID = 15
			};

			bool myIsFin;
			Opcode myOpCode;

			std::vector<uint8_t> myPayload;
		};

		bool TryParseFrame();
		void OnFrame();

		void Defragment();

		void HandleFrame();

		void SendFrame(Frame::Opcode aOpCode, const std::vector<uint8_t>& aPayload);

		void Fail(std::string aReason);


		Frame myReusedFrame;

		bool myHasFailed;
		bool myHasClosed;

		ReadStream& myReadStream;
		WriteStream& myWriteStream;

		Frame::Opcode myFragmentedOpCode;
		uint64_t myTotalFragmentSize;
		std::vector<std::vector<uint8_t>> myFragments;
	};
}
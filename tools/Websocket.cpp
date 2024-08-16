
#include "tools/Websocket.h"
#include "tools/StreamReader.h"
#include "tools/StreamWriter.h"

#include <cstring>

namespace fisk::tools
{
	Websocket::Websocket(ReadStream& aReadStream, WriteStream& aWriteStream)
		: myFragmentedOpCode(Frame::Opcode::INVALID)
		, myTotalFragmentSize(0)
		, myReadStream(aReadStream)
		, myWriteStream(aWriteStream)
	{
	}

	bool Websocket::Update()
	{
		if (myHasClosed)
			return false;

		if (myHasFailed)
			return false;

		while (TryParseFrame());

		return true;
	}

	void Websocket::SendText(std::string aData)
	{
		std::vector<uint8_t> rawData;

		rawData.resize(aData.length());
		memcpy(rawData.data(), aData.data(), aData.length());

		SendFrame(Frame::Opcode::Text, rawData);
	}

	bool Websocket::TryParseFrame()
	{
		myReadStream.RestoreRead();

		StreamReader reader(myReadStream);

		uint8_t headerByte;

		if (!reader.Process(headerByte))
			return false;

		uint8_t maskAndSmallLength;
		if (!reader.Process(maskAndSmallLength))
			return false;

		myReusedFrame.myIsFin = !!(headerByte & 0x80);
		myReusedFrame.myOpCode = static_cast<Frame::Opcode>(headerByte & 0x0F);

		bool masked = !!(maskAndSmallLength & 0x80);

		uint8_t shortLength = maskAndSmallLength & 0x7F;
		
		uint64_t actualLength = 0;

		switch (shortLength)
		{
		case 127:
			{
				uint64_t trueLength;
				if (!reader.Process(trueLength))
					return false;

				if (trueLength & (uint64_t{ 1 } << 63))
				{
					Fail("Malformed length");
					return false; // MSB must be zero
				}

				actualLength = trueLength;
			}
			break;
		case 126:
			{
				uint16_t trueLength;
				if (!reader.Process(trueLength))
					return false;

				actualLength = trueLength;
			}
			break;
		default:
			actualLength = shortLength;
			break;
		}

		uint8_t mask[4] = { 0x00, 0x00, 0x00, 0x00 };

		if (masked)
		{
			if (!myReadStream.Read(mask, 4))
				return false;
		}

		myReusedFrame.myPayload.resize(actualLength);

		if (!myReadStream.Read(myReusedFrame.myPayload.data(), actualLength))
			return false;

		myReadStream.CommitRead();

		OnFrame();

		return !myHasFailed;
	}

	void Websocket::OnFrame()
	{
		if (!myReusedFrame.myIsFin || (myReusedFrame.myOpCode == Frame::Opcode::Continuation))
		{
			Defragment();
			return;
		}

		HandleFrame();
	}

	void Websocket::Defragment()
	{
		if (myReusedFrame.myIsFin)
		{
			myReusedFrame.myOpCode = myFragmentedOpCode;
			std::vector<uint8_t> finalChunk(std::move(myReusedFrame.myPayload));

			myReusedFrame.myPayload = std::move(myFragments.front());

			uint64_t at = myReusedFrame.myPayload.size();
			myReusedFrame.myPayload.resize(myTotalFragmentSize);

			if (myFragments.empty())
			{
				Fail("Fin was sent without other fragments");
				return;
			}

			for (size_t i = 1; i < myFragments.size(); i++)
			{
				std::vector<uint8_t>& fragment = myFragments[i];

				if (myTotalFragmentSize < fragment.size())
				{
					Fail("Single fragment was larger than total fragment size");
					return;
				}

				if (myTotalFragmentSize - fragment.size() < at)
				{
					Fail("Fragments overran buffer");
					return;
				}

				::memcpy(myReusedFrame.myPayload.data() + at, fragment.data(), fragment.size());

				at += fragment.size();
			}

			if (at != myTotalFragmentSize)
			{
				Fail("Fragments didn't sum to the total");
				return;
			}

			myFragmentedOpCode = Frame::Opcode::INVALID;
			myTotalFragmentSize = 0;
			myFragments.clear();

			HandleFrame();
			return;
		}

		if (myReusedFrame.myOpCode != Frame::Opcode::Continuation)
		{
			myFragmentedOpCode = myReusedFrame.myOpCode;
		}

		myFragments.emplace_back(std::move(myReusedFrame.myPayload));

		myReusedFrame.myPayload.clear();
	}

	void Websocket::HandleFrame()
	{
		switch (myReusedFrame.myOpCode)
		{
		case Frame::Opcode::Text:
			{
				Blob blob;
				blob.myType = Blob::Type::Text;
				blob.myData = &myReusedFrame.myPayload;
				OnData.Fire(blob);
			}
			break;
		case Frame::Opcode::Binary:
			{
				Blob blob;
				blob.myType = Blob::Type::Binary;
				blob.myData = &myReusedFrame.myPayload;
				OnData.Fire(blob);
			}
			break;
		case Frame::Opcode::Close:
			{
				if (!myHasClosed)
					SendFrame(Frame::Opcode::Close, myReusedFrame.myPayload);
			}
			break;
		case Frame::Ping:
			{
				if (!myHasClosed)
					SendFrame(Frame::Opcode::Pong, myReusedFrame.myPayload);
			}
			break;
		case Frame::Pong:
			break;
		case Frame::Opcode::Continuation:
			Fail("Continuation should not show up in this context");
			break;
		default:
			Fail("Unkown op code");
			break;
		}
	}

	void Websocket::SendFrame(Frame::Opcode aOpCode, const std::vector<uint8_t>& aPayload)
	{
		StreamWriter writer(myWriteStream);

		uint8_t headerByte = 0x80 | aOpCode;
		writer.Process(headerByte);

		uint8_t masked = 0x00;

		if (aPayload.size() > std::numeric_limits<uint16_t>::max())
		{
			uint64_t length = aPayload.size();
			uint8_t shortLength = masked | 0x8F;

			writer.Process(shortLength);
			writer.Process(length);
		}
		else if (aPayload.size() > 125)
		{
			uint16_t length = aPayload.size();
			uint8_t shortLength = masked | 0x8E;

			writer.Process(shortLength);
			writer.Process(length);
		}
		else
		{
			uint8_t shortLength = masked | static_cast<uint8_t>(aPayload.size());

			writer.Process(shortLength);
		}

		// TODO: masking if client

		myWriteStream.WriteData(aPayload.data(), aPayload.size());
	}

	void Websocket::Fail(std::string aReason)
	{
		myHasFailed = true;
	}
}

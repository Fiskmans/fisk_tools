#include "tools/StreamReader.h"

namespace fisk::tools
{

	bool StreamReader::ReadRaw(uint8_t* aData, uint32_t aSize)
	{
		return myReadStream.Read(aData, aSize);
	}

	StreamReader::StreamReader(ReadStream& aReadStream)
		: myReadStream(aReadStream)
	{
	}

	bool StreamReader::Process(uint8_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int8_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(uint16_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int16_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(uint32_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int32_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(uint64_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	bool StreamReader::Process(int64_t& aValue)
	{
		return ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

} // namespace fisk::tools
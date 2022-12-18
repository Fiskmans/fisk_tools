#include "tools/StreamWriter.h"

namespace fisk::tools
{
	StreamWriter::StreamWriter(WriteStream& aStream)
		: myStream(aStream)
	{
	}

	bool StreamWriter::Process(uint8_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int8_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(uint16_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int16_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(uint32_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int32_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(uint64_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	bool StreamWriter::Process(int64_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
		return true;
	}

	void StreamWriter::WriteRaw(uint8_t* aData, uint32_t aSize)
	{
		myStream.WriteData(aData, aSize);
	}

} // namespace fisk::tools
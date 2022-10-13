#include "tools/StreamReader.h"

namespace fisk::tools {

	void StreamReader::ReadRaw(uint8_t* aData, uint32_t aSize)
	{
		//TODO 2
	}

	void StreamReader::Process(uint8_t& aValue)
	{
		ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	void StreamReader::Process(int8_t& aValue)
	{
		ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}


	void StreamReader::Process(uint16_t& aValue)
	{
		ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	void StreamReader::Process(int16_t& aValue)
	{
		ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}


	void StreamReader::Process(uint32_t& aValue)
	{
		ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	void StreamReader::Process(int32_t& aValue)
	{
		ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}


	void StreamReader::Process(uint64_t& aValue)
	{
		ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	void StreamReader::Process(int64_t& aValue)
	{
		ReadRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

} // fisk::tools
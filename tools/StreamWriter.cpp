#include "tools/StreamWriter.h"

namespace fisk::tools {

	void StreamWriter::WriteRaw(uint8_t* aData, uint32_t aSize)
	{
		//TODO 1
	}

	void StreamWriter::Process(uint8_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	void StreamWriter::Process(int8_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}


	void StreamWriter::Process(uint16_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	void StreamWriter::Process(int16_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}


	void StreamWriter::Process(uint32_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	void StreamWriter::Process(int32_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}


	void StreamWriter::Process(uint64_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

	void StreamWriter::Process(int64_t& aValue)
	{
		WriteRaw(reinterpret_cast<uint8_t*>(&aValue), sizeof(aValue));
	}

} // fisk::tools
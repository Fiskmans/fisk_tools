#ifndef FISK_TOOLS_DATA_PROCESSOR_H
#define FISK_TOOLS_DATA_PROCESSOR_H

#include <cstdint>

namespace fisk::tools {

	class DataProcessor
	{
		virtual void Process(uint8_t& aValue) = 0;
		virtual void Process(int8_t& aValue) = 0;

		virtual void Process(uint16_t& aValue) = 0;
		virtual void Process(int16_t& aValue) = 0;

		virtual void Process(uint32_t& aValue) = 0;
		virtual void Process(int32_t& aValue) = 0;

		virtual void Process(uint64_t& aValue) = 0;
		virtual void Process(int64_t& aValue) = 0;
	};

} // fisk::tools

#endif
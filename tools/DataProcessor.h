#ifndef FISK_TOOLS_DATA_PROCESSOR_H
#define FISK_TOOLS_DATA_PROCESSOR_H

#include <cstdint>

namespace fisk::tools
{

    class DataProcessor
    {
        virtual bool Process(uint8_t& aValue) = 0;
        virtual bool Process(int8_t& aValue)  = 0;

        virtual bool Process(uint16_t& aValue) = 0;
        virtual bool Process(int16_t& aValue)  = 0;

        virtual bool Process(uint32_t& aValue) = 0;
        virtual bool Process(int32_t& aValue)  = 0;

        virtual bool Process(uint64_t& aValue) = 0;
        virtual bool Process(int64_t& aValue)  = 0;
    };

} // namespace fisk::tools

#endif
#ifndef FISK_TOOLS_STREAM_READER_H
#define FISK_TOOLS_STREAM_READER_H

#include "tools/DataProcessor.h"

namespace fisk::tools
{

    class StreamReader : public DataProcessor
    {
        void ReadRaw(uint8_t* aData, uint32_t aSize);

        void Process(uint8_t& aValue) override;
        void Process(int8_t& aValue) override;

        void Process(uint16_t& aValue) override;
        void Process(int16_t& aValue) override;

        void Process(uint32_t& aValue) override;
        void Process(int32_t& aValue) override;

        void Process(uint64_t& aValue) override;
        void Process(int64_t& aValue) override;
    };
} // namespace fisk::tools

#endif
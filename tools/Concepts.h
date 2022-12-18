#ifndef FISK_TOOLS_CONCEPTS_H
#define FISK_TOOLS_CONCEPTS_H

#include "tools/DataProcessor.h"

#include <concepts>

namespace fisk::tools
{
    template <class Type>
    concept Serializable = requires(Type aItem)
    {
        {aItem.Process(std::declval<DataProcessor>())};
    };

} // namespace fisk::tools

#endif
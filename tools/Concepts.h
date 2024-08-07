#ifndef FISK_TOOLS_CONCEPTS_H
#define FISK_TOOLS_CONCEPTS_H

#include <concepts>
#include <limits>

namespace fisk::tools
{
    class DataProcessor;

    template <class Type>
    concept Serializable = requires(Type aItem)
    {
        requires std::is_class_v<Type>;

        { aItem.Process(std::declval<DataProcessor&>()) } -> std::convertible_to<bool>;
    };


    template<class T>
    concept Numeric = requires()
    {
        requires std::numeric_limits<T>::is_specialized;
    };

    template<class T>
    concept EnumType = requires()
    {
        requires std::is_enum<T>::value;
    };

} // namespace fisk::tools

#endif
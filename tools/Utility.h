#ifndef TOOLS_UTILITY_H
#define TOOLS_UTILITY_H

namespace fisk::tools
{
	template<typename Type>
	inline bool Compare(const Type aLHS, const Type aRHS)
	{
		return aLHS == aRHS;
	}

	template<typename FromType, typename ToType>
	inline ToType Convert(const FromType aSource)
	{
		return static_cast<ToType>(aSource);
	}

	template<typename Type>
	inline Type Square(const Type aValue)
	{
		return aValue * aValue;
	}

	template <typename Type, size_t Number>
	using ExtractTypeFromUnpackedNumber = Type;
}

#endif
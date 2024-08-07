
#pragma once

#include "tools/Concepts.h"
#include "tools/DataProcessor.h"

#include <limits>

namespace fisk::tools
{
	class SystemValues
	{
	public:
		bool Process(DataProcessor& aProcessor);

		bool operator==(const SystemValues&) const = default;

		template<Numeric T>
		struct NumericInfo
		{
			bool operator==(const NumericInfo&) const = default;

			bool Process(DataProcessor& aProcessor);
			
			uint32_t	mySize				= sizeof(T);

			uint8_t		myIsSigned			= std::numeric_limits<T>::is_signed;
			uint8_t		myHasInfinity		= std::numeric_limits<T>::has_infinity;
			uint8_t		myHasQuietNan		= std::numeric_limits<T>::has_quiet_NaN;
			uint8_t		myHasSignalingNan	= std::numeric_limits<T>::has_signaling_NaN;
			int32_t		myHasDenorm			= std::numeric_limits<T>::has_denorm;
			uint8_t		myHasDenormLoss		= std::numeric_limits<T>::has_denorm_loss;
			int32_t		myRoundStyle		= std::numeric_limits<T>::round_style;
			uint8_t		myIsIEC559			= std::numeric_limits<T>::is_iec559;
			uint8_t		myIsModulo			= std::numeric_limits<T>::is_modulo;
			uint32_t	myDigits			= std::numeric_limits<T>::digits;
			uint32_t	myDigits10			= std::numeric_limits<T>::digits10;
			uint32_t	myMaxDigits10		= std::numeric_limits<T>::max_digits10;
			uint32_t	myRadix				= std::numeric_limits<T>::radix;
			int32_t		myMinExponent		= std::numeric_limits<T>::min_exponent;
			int32_t		myMinExponent10		= std::numeric_limits<T>::min_exponent10;
			uint32_t	myMaxExponent		= std::numeric_limits<T>::max_exponent;
			uint32_t	myMaxExponent10		= std::numeric_limits<T>::max_exponent10;
			uint8_t		myTraps				= std::numeric_limits<T>::traps;
			uint8_t		myTinynessBefore	= std::numeric_limits<T>::tinyness_before;
		};

		NumericInfo<float>			myFloatValues;
		NumericInfo<double>			myDoubleValues;
		NumericInfo<long double>	myLongDoubleValues;

		NumericInfo<char>			myCharValues;
		NumericInfo<short>			myShortValues;
		NumericInfo<int>			myIntValues;
		NumericInfo<long>			myLongValues;
		NumericInfo<long long>		myLongLongValues;
		NumericInfo<size_t>			mySizeTValues;
	};

	template<Numeric T>
	inline bool SystemValues::NumericInfo<T>::Process(DataProcessor& aProcessor)
	{
		return aProcessor.Process(mySize)
			&& aProcessor.Process(myIsSigned)
			&& aProcessor.Process(myHasInfinity)
			&& aProcessor.Process(myHasQuietNan)
			&& aProcessor.Process(myHasSignalingNan)
			&& aProcessor.Process(myHasDenorm)
			&& aProcessor.Process(myHasDenormLoss)
			&& aProcessor.Process(myRoundStyle)
			&& aProcessor.Process(myIsIEC559)
			&& aProcessor.Process(myIsModulo)
			&& aProcessor.Process(myDigits)
			&& aProcessor.Process(myDigits10)
			&& aProcessor.Process(myMaxDigits10)
			&& aProcessor.Process(myMaxDigits10)
			&& aProcessor.Process(myMinExponent)
			&& aProcessor.Process(myMinExponent10)
			&& aProcessor.Process(myMaxExponent)
			&& aProcessor.Process(myMaxExponent10)
			&& aProcessor.Process(myTraps)
			&& aProcessor.Process(myTinynessBefore);
	}
}
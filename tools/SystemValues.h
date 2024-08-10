
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

		bool operator==(const SystemValues&) const = delete;

		
		struct Difference
		{
			operator bool();

			std::string ToString();

			std::string myTag;
			std::string myMessage;
			std::vector<std::unique_ptr<Difference>> mySubDifferences;
		};

		Difference Differences(SystemValues& aOther);


		template<Numeric T>
		struct NumericInfo
		{
			bool operator==(const NumericInfo&) const = default;

			bool Process(DataProcessor& aProcessor);

			void AddDifferences(Difference& aOut, NumericInfo& aOther);

			template<typename ValueType>
			void CheckForDifferences(Difference& aOut, std::string aTag, ValueType aLocal, ValueType aRemote);
			
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

		template<Numeric T>
		void CheckForDifferences(Difference& aOut, std::string aTag, NumericInfo<T>& aLocal, NumericInfo<T>& aRemote);

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
			&& aProcessor.Process(myRadix)
			&& aProcessor.Process(myMinExponent)
			&& aProcessor.Process(myMinExponent10)
			&& aProcessor.Process(myMaxExponent)
			&& aProcessor.Process(myMaxExponent10)
			&& aProcessor.Process(myTraps)
			&& aProcessor.Process(myTinynessBefore);
	}

	template<Numeric T>
	inline void SystemValues::NumericInfo<T>::AddDifferences(Difference& aOut, NumericInfo& aOther)
	{
		CheckForDifferences(aOut, "size", mySize, aOther.mySize);
		CheckForDifferences(aOut, "signed", myIsSigned, aOther.myIsSigned);
		CheckForDifferences(aOut, "infinity", myHasInfinity, aOther.myHasInfinity);
		CheckForDifferences(aOut, "quiet_nan", myHasQuietNan, aOther.myHasQuietNan);
		CheckForDifferences(aOut, "signaling_nan", myHasSignalingNan, aOther.myHasSignalingNan);
		CheckForDifferences(aOut, "denorm", myHasDenorm, aOther.myHasDenorm);
		CheckForDifferences(aOut, "denorm_loss", myHasDenormLoss, aOther.myHasDenormLoss);
		CheckForDifferences(aOut, "roundstyle", myRoundStyle, aOther.myRoundStyle);
		CheckForDifferences(aOut, "iec_559", myIsIEC559, aOther.myIsIEC559);
		CheckForDifferences(aOut, "modulo", myIsModulo, aOther.myIsModulo);
		CheckForDifferences(aOut, "digits", myDigits, aOther.myDigits);
		CheckForDifferences(aOut, "digits_10", myDigits10, aOther.myDigits10);
		CheckForDifferences(aOut, "max_digits_10", myMaxDigits10, aOther.myMaxDigits10);
		CheckForDifferences(aOut, "radix", myRadix, aOther.myRadix);
		CheckForDifferences(aOut, "min_exponent", myMinExponent, aOther.myMinExponent);
		CheckForDifferences(aOut, "min_exponent_10", myMinExponent10, aOther.myMinExponent10);
		CheckForDifferences(aOut, "max_exponent", myMaxExponent, aOther.myMaxExponent);
		CheckForDifferences(aOut, "max_exponent_10", myMaxExponent10, aOther.myMaxExponent10);
		CheckForDifferences(aOut, "traps", myTraps, aOther.myTraps);
		CheckForDifferences(aOut, "tinyness_before", myTinynessBefore, aOther.myTinynessBefore);
	}

	template<Numeric T>
	template<typename ValueType>
	inline void SystemValues::NumericInfo<T>::CheckForDifferences(Difference& aOut, std::string aTag, ValueType aLocal, ValueType aRemote)
	{
		if (aLocal == aRemote)
			return;

		std::unique_ptr<Difference> subDiff = std::make_unique<Difference>();

		subDiff->myTag = aTag;
		subDiff->myMessage = "local: " + std::to_string(aLocal) + " remote: " + std::to_string(aRemote);

		aOut.mySubDifferences.push_back(std::move(subDiff));
	}

	template<Numeric T>
	inline void SystemValues::CheckForDifferences(Difference& aOut, std::string aTag, NumericInfo<T>& aLocal, NumericInfo<T>& aRemote)
	{
		if (aLocal == aRemote)
			return;

		std::unique_ptr<Difference> subDiff = std::make_unique<Difference>();

		subDiff->myTag = aTag;
		subDiff->myMessage = "[" + aTag + "]";

		aOut.mySubDifferences.push_back(std::move(subDiff));
	}
}
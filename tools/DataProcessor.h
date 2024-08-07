#ifndef FISK_TOOLS_DATA_PROCESSOR_H
#define FISK_TOOLS_DATA_PROCESSOR_H

#include "tools/Concepts.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <utility>
#include <tuple>
#include <chrono>
#include <optional>

namespace fisk::tools
{

	class DataProcessor
	{
	public:
		virtual bool Process(bool& aValue)    = 0;

		virtual bool Process(uint8_t& aValue) = 0;
		virtual bool Process(int8_t& aValue)  = 0;

		virtual bool Process(uint16_t& aValue) = 0;
		virtual bool Process(int16_t& aValue)  = 0;

		virtual bool Process(uint32_t& aValue) = 0;
		virtual bool Process(int32_t& aValue)  = 0;

		virtual bool Process(uint64_t& aValue) = 0;
		virtual bool Process(int64_t& aValue)  = 0;

		virtual bool Process(float& aValue)    = 0;
		virtual bool Process(double& aValue)   = 0;

		virtual bool Process(std::string& aValue) = 0;

		template<class T>
		bool Process(std::vector<T>& aVector);

		template<class T>
		bool Process(std::unique_ptr<T>& aPtr);

		template<class T>
		bool Process(std::optional<T>& aOptional);

		template<class T, class Ratio>
		bool Process(std::chrono::duration<T, Ratio>& aDuration);

		template<class Left, class Right>
		bool Process(std::pair<Left, Right>& aPair);

		template<class... Types>
		bool Process(std::tuple<Types...>& aTuple);

		template<Serializable T>
		bool Process(T& aValue);

		template<EnumType T>
		bool Process(T& aValue);

	private:
		template<class Tuple, size_t... IndexSequence>
		bool ProcessTuple(Tuple& aTuple, std::index_sequence<IndexSequence...>);
	};

	template<class T>
	inline bool DataProcessor::Process(std::vector<T>& aVector)
	{
		size_t size = aVector.size();

		if (!this->Process(size))
			return false;

		aVector.resize(size);

		for (T& item : aVector)
			if (!this->Process(item))
				return false;

		return true;
	}

	template<class T>
	inline bool DataProcessor::Process(std::unique_ptr<T>& aPtr)
	{
		bool hasItem = !!aPtr;

		if (!this->Process(hasItem))
			return false;

		if (!hasItem)
		{
			aPtr = nullptr;
			return true;
		}

		if (!aPtr)
			aPtr = std::make_unique<T>();

		return this->Process(*aPtr);
	}

	template<class T>
	inline bool DataProcessor::Process(std::optional<T>& aOptional)
	{
		bool hasItem = !!aOptional;

		if (!this->Process(hasItem))
			return false;

		if (!hasItem)
		{
			aOptional.reset();
			return true;
		}

		if (!aOptional)
			aOptional.emplace();

		return this->Process(*aOptional);
	}

	template<class T, class Ratio>
	inline bool DataProcessor::Process(std::chrono::duration<T, Ratio>& aDuration)
	{
		T val = aDuration.count();
		if (Process(val))
		{
			aDuration = std::chrono::duration<T, Ratio>{val};
			return true;
		}
		return false;
	}

	template<class Left, class Right>
	inline bool DataProcessor::Process(std::pair<Left, Right>& aPair)
	{
		return Process(aPair.first)
			&& Process(aPair.second);
	}

	template<class... Types>
	inline bool DataProcessor::Process(std::tuple<Types...>& aTuple)
	{
		return ProcessTuple(aTuple, std::make_index_sequence<sizeof...(Types)>{});
	}

	template<class Tuple, size_t... IndexSequence>
	inline bool DataProcessor::ProcessTuple(Tuple& aTuple, std::index_sequence<IndexSequence...>)
	{
		return (Process(std::get<IndexSequence>(aTuple)) && ...);
	}

	template<Serializable T>
	inline bool DataProcessor::Process(T& aValue)
	{
		return aValue.Process(*this);
	}

	template<EnumType T>
	inline bool DataProcessor::Process(T& aValue)
	{
		using UnderLying = std::underlying_type_t<T>;

		UnderLying val = static_cast<UnderLying>(aValue);

		if (Process(val))
		{
			aValue = static_cast<T>(val);
			return true;
		}

		return false;
	}

} // namespace fisk::tools

#endif
#ifndef FISK_TOOLS_DATA_PROCESSOR_H
#define FISK_TOOLS_DATA_PROCESSOR_H

#include "tools/Concepts.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

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
		bool Process(std::unique_ptr<T>& aVector);

		template<Serializable T>
		bool Process(T& aValue);
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

	template<Serializable T>
	inline bool DataProcessor::Process(T& aValue)
	{
		return aValue.Process(*this);
	}

} // namespace fisk::tools

#endif
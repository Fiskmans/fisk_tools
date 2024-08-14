
#ifndef FISK_TOOLS_JSON_H
#define FISK_TOOLS_JSON_H

#include <concepts>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace fisk::tools
{
	class Json;
	class JsonObjectIterator
	{
	public:
		using DereferenceType = std::pair<const std::string, Json&>;
		using BaseIterator	  = std::unordered_map<std::string, std::unique_ptr<Json>>::iterator;

		JsonObjectIterator();
		JsonObjectIterator(BaseIterator aIterator);

		DereferenceType operator*();
		void operator++();
		void operator++(int);

		bool operator==(const JsonObjectIterator& aOther) const;

		bool myIsValid = true;
		BaseIterator myIterator;
	};

	class JsonObjectProxy
	{
	public:
		JsonObjectProxy(std::unordered_map<std::string, std::unique_ptr<Json>>* aContainer);

		JsonObjectIterator begin();
		JsonObjectIterator end();

	private:
		std::unordered_map<std::string, std::unique_ptr<Json>>* myContainer;
	};

	class JsonArrayIterator
	{
	public:
		using DereferenceType = Json&;
		using BaseIterator	  = std::vector<std::unique_ptr<Json>>::iterator;

		JsonArrayIterator();
		JsonArrayIterator(BaseIterator aIterator);

		DereferenceType operator*();
		void operator++();
		void operator++(int);

		bool operator==(const JsonArrayIterator& aOther) const;

		bool myIsValid = true;
		BaseIterator myIterator;
	};

	class JsonArrayProxy
	{
	public:
		JsonArrayProxy(std::vector<std::unique_ptr<Json>>* aContainer);

		JsonArrayIterator begin();
		JsonArrayIterator end();

	private:
		std::vector<std::unique_ptr<Json>>* myContainer;
	};

	class Json
	{
	public:
		using NullType	  = nullptr_t;
		using NumberType  = double;
		using StringType  = std::string;
		using BooleanType = bool;
		using ArrayType	  = std::vector<std::unique_ptr<Json>>;
		using ObjectType  = std::unordered_map<std::string, std::unique_ptr<Json>>;

		~Json()								= default;
		Json()								= default;
		Json(const Json& aOther)			= delete;
		void operator=(const Json& aOther)	= delete;
		void operator==(const Json& aOther) = delete;

		Json& operator[](const char* aKey) const;
		Json& operator[](int aIndex) const;

		bool Parse(const char* aString);

		template<class T>
		Json& AddValue(const std::string& aKey, const T& aValue);
		Json& AddChild(const std::string& aKey, std::unique_ptr<Json> aChild = std::make_unique<Json>());

		template<class T>
		Json& PushValue(const T& aValue);
		Json& PushChild(std::unique_ptr<Json> aChild = std::make_unique<Json>());

		bool HasChild(const char* aKey) const;

		bool IsNull() const;
		operator bool() const;

		std::string Serialize(bool aPretty = false) const;
		void Serialize(std::ostream& aOutStream, bool aPretty = false, std::string aNewline = "") const;

		template<class T>
		Json& operator=(T aValue);

		bool GetIf(long long& aValue) const;
		bool GetIf(long& aValue) const;
		bool GetIf(size_t& aValue) const;
		bool GetIf(int& aValue) const;
		bool GetIf(uint8_t& aValue) const;
		bool GetIf(double& aValue) const;
		bool GetIf(float& aValue) const;
		bool GetIf(std::string& aValue) const;
		bool GetIf(bool& aValue) const;

		JsonObjectProxy IterateObject();
		JsonArrayProxy IterateArray();

	private:
		static Json NullObject;

		bool ParseInternal(const char*& aString);
		bool ParseAsValue(const char*& aBegin);

		using AnyType = std::variant<NullType, NumberType, StringType, BooleanType, ArrayType, ObjectType>;

		AnyType myValue;
	};

	template <class T>
	inline Json& tools::Json::AddValue(const std::string& aKey, const T& aValue)
	{
		if (this == &NullObject)
			return NullObject;

		std::unique_ptr<Json> child = std::make_unique<Json>();
		*child						= aValue;
		Json& out					= *child;
		AddChild(aKey, std::move(child));
		return out;
	}

	template <class T>
	inline Json& tools::Json::PushValue(const T& aValue)
	{
		if (this == &NullObject)
			return NullObject;

		std::unique_ptr<Json> child = std::make_unique<Json>();
		*child						= aValue;
		Json& out					= *child;
		PushChild(std::move(child));
		return out;
	}

	template <>
	inline Json& Json::operator=<Json::BooleanType>(BooleanType aValue)
	{
		if (this != &NullObject)
			myValue = aValue;

		return *this;
	}
	
	template <>
	inline Json& Json::operator=<const Json::StringType&>(const StringType& aValue)
	{
		if (this != &NullObject)
			myValue = aValue;

		return *this;
	}

	template <>
	inline Json& Json::operator=<Json::StringType>(StringType aValue)
	{
		if (this != &NullObject)
			myValue = aValue;

		return *this;
	}

	template <>
	inline Json& Json::operator=<const char*>(const char* aValue)
	{
		if (this != &NullObject)
			myValue = aValue;

		return *this;
	}

	template <class T>
	inline Json& Json::operator=(T aValue)
	{
		static_assert(!std::is_same_v<T, BooleanType>);

		if (this != &NullObject)
			myValue = static_cast<NumberType>(aValue);

		return *this;
	}

} // namespace fisk::tools

#endif
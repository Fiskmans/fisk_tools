
#ifndef FISK_TOOLS_JSON_H
#define FISK_TOOLS_JSON_H

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace fisk::tools
{
	class JSONObject
	{
	public:
		using NullType	  = nullptr_t;
		using NumberType  = double;
		using StringType  = std::string;
		using BooleanType = bool;
		using ArrayType	  = std::vector<std::unique_ptr<JSONObject>>;
		using ObjectType  = std::unordered_map<std::string, std::unique_ptr<JSONObject>>;

		~JSONObject()							  = default;
		JSONObject()							  = default;
		JSONObject(const JSONObject& aOther)	  = delete;
		void operator=(const JSONObject& aOther)  = delete;
		void operator==(const JSONObject& aOther) = delete;

		JSONObject& operator[](const char* aKey) const;
		JSONObject& operator[](size_t aIndex) const;

		bool Parse(const char* aString);

		bool HasChild(const char* aKey) const;

		template <typename T>
		void AddValue(const std::string& aKey, T aValue);
		void AddChild(const std::string& aKey, std::unique_ptr<JSONObject> aChild);

		template <typename T>
		void PushValue(T aValue);
		void PushChild(std::unique_ptr<JSONObject> aChild);

		bool IsNull() const;
		operator bool() const;

		std::string Serialize(bool aPretty = false);

		JSONObject& operator=(const NumberType& aValue);
		JSONObject& operator=(const StringType& aValue);
		JSONObject& operator=(const BooleanType& aValue);

		bool GetIf(long long& aValue) const;
		bool GetIf(long& aValue) const;
		bool GetIf(size_t& aValue) const;
		bool GetIf(int& aValue) const;
		bool GetIf(double& aValue) const;
		bool GetIf(float& aValue) const;
		bool GetIf(std::string& aValue) const;
		bool GetIf(bool& aValue) const;
		bool GetIf(ArrayType*& aValue);
		bool GetIf(ObjectType*& aValue);

	private:
		static JSONObject NullObject;

		bool ParseInternal(const char*& aString);
		bool ParseAsValue(const char*& aBegin);

		using AnyType = std::variant<NullType, NumberType, StringType, BooleanType, ArrayType, ObjectType>;

		AnyType myValue;
	};

	template <typename T>
	inline void tools::JSONObject::AddValue(const std::string& aKey, T aValue)
	{
		std::unique_ptr<JSONObject> child = std::make_unique<JSONObject>();
		child							  = aValue;
		AddChild(aKey, child);
	}

	template <typename T>
	inline void tools::JSONObject::PushValue(T aValue)
	{
		std::unique_ptr<JSONObject> child = std::make_unique<JSONObject>();
		child							  = aValue;
		PushChild(child);
	}

} // namespace fisk::tools

#endif
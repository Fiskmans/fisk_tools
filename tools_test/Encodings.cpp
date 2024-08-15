
#include <catch2/catch_all.hpp>

#include "tools/Base64.h"

TEST_CASE("Base64", "[Encoding]")
{
	using namespace fisk::tools;

	const char* testData = "Many hands make light work.";

	size_t length = strlen(testData);

	std::vector<uint8_t> data;

	data.resize(length);
	memcpy(data.data(), testData, length);

	REQUIRE(Base64::Encode(data) == "TWFueSBoYW5kcyBtYWtlIGxpZ2h0IHdvcmsu");

	REQUIRE(Base64::Encode({ 'M', 'a', 'n' }) == "TWFu");
	REQUIRE(Base64::Encode({ 'M', 'a' }) == "TWE=");
	REQUIRE(Base64::Encode({ 'M' }) == "TQ==");

	REQUIRE(Base64::Encode(Base64::Decode("TWFu")) == "TWFu");
	REQUIRE(Base64::Encode(Base64::Decode("TWE=")) == "TWE=");
	REQUIRE(Base64::Encode(Base64::Decode("TQ==")) == "TQ==");
}
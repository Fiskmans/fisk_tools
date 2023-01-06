

#include <catch2/catch_all.hpp>

#include "tools/Json.h"
#include "tools/StreamReader.h"
#include "tools/StreamWriter.h"

#include <algorithm>
#include <string_view>

TEST_CASE("WriteStream", "[Data]")
{
	SECTION("Basic")
	{
		fisk::tools::WriteStream ws;

		REQUIRE(!ws.Get());

		const uint8_t data[8]{1, 2, 3, 4, 5, 6, 7, 8};

		REQUIRE(!ws.HasData());

		ws.WriteData(data, 8);

		REQUIRE(ws.HasData());

		std::shared_ptr<fisk::tools::StreamSegment> streamData = ws.Get();

		REQUIRE(!ws.HasData());
		REQUIRE(streamData);
		REQUIRE(!streamData->myNext);
		REQUIRE(streamData->mySize == 8);
		REQUIRE(memcmp(streamData->myData, data, 8) == 0);

		ws.WriteData(data, 8);

		REQUIRE(ws.HasData());

		streamData = ws.Get();

		REQUIRE(!ws.HasData());
		REQUIRE(streamData);
		REQUIRE(!streamData->myNext);
		REQUIRE(streamData->mySize == 8);
		REQUIRE(memcmp(streamData->myData, data, 8) == 0);
	}

	{
		fisk::tools::WriteStream ws;

		size_t left = fisk::tools::StreamSegment::CHUNK_SIZE;
		while (left > 0)
		{
			size_t chunk = std::min(left, 8ull);

			const uint8_t data[8]{1, 2, 3, 4, 5, 6, 7, 8};

			ws.WriteData(data, chunk);
			left -= chunk;
		}

		SECTION("Exactly full")
		{
			std::shared_ptr<fisk::tools::StreamSegment> streamData = ws.Get();

			REQUIRE(!streamData->myNext);
			REQUIRE(streamData->mySize == fisk::tools::StreamSegment::CHUNK_SIZE);
			REQUIRE(streamData->SpaceLeft() == 0);
		}

		SECTION("Spilling over")
		{
			const uint8_t secondData[] = {9, 10, 11, 12, 13, 14, 15};
			ws.WriteData(secondData, 8);

			std::shared_ptr<fisk::tools::StreamSegment> streamData = ws.Get();

			REQUIRE(streamData->myNext);
			REQUIRE(streamData->mySize == fisk::tools::StreamSegment::CHUNK_SIZE);
			REQUIRE(streamData->SpaceLeft() == 0);

			std::shared_ptr<fisk::tools::StreamSegment> next = streamData->myNext;

			REQUIRE(!next->myNext);
			REQUIRE(next->mySize == 8);
			REQUIRE(memcmp(next->myData, secondData, 8) == 0);
		}
	}
}

TEST_CASE("StreamWriter", "[Data]")
{
	fisk::tools::WriteStream ws;
	fisk::tools::StreamWriter sw(ws);

	uint8_t u8	 = 1;
	int8_t i8	 = -2;
	uint16_t u16 = 3;
	int16_t i16	 = -4;
	uint32_t u32 = 5;
	int32_t i32	 = -6;
	uint64_t u64 = 7;
	int64_t i64	 = -8;

	std::string s = "Hello";

	REQUIRE(sw.Process(u8));
	REQUIRE(sw.Process(i8));
	REQUIRE(sw.Process(u16));
	REQUIRE(sw.Process(i16));
	REQUIRE(sw.Process(u32));
	REQUIRE(sw.Process(i32));
	REQUIRE(sw.Process(u64));
	REQUIRE(sw.Process(i64));
	REQUIRE(sw.Process(s));

	std::shared_ptr<fisk::tools::StreamSegment> head = ws.Get();

	const uint8_t expected[] = {
		// clang-format off
		0b00000001,
		0b11111110,
		0b00000000, 0b00000011,
		0b11111111, 0b11111100,
		0b00000000, 0b00000000, 0b00000000, 0b00000101,
		0b11111111, 0b11111111, 0b11111111, 0b11111010,
		0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000111,
		0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111000,
		'H','e','l','l','o','\0'
		// clang-format on
	};

	REQUIRE(!head->myNext);
	REQUIRE(head->mySize == sizeof(expected));
	REQUIRE(memcmp(head->myData, expected, sizeof(expected)) == 0);
}

TEST_CASE("ReadStream", "[Data]")
{
	std::shared_ptr<fisk::tools::StreamSegment> segment = std::make_shared<fisk::tools::StreamSegment>();

	const uint8_t data[8]{1, 2, 3, 4, 5, 6, 7, 8};
	segment->mySize = 8;
	memcpy(segment->myData, data, 8);

	fisk::tools::ReadStream rs;

	rs.AppendData(segment);

	uint8_t read[16];
	REQUIRE(rs.Read(read, 8));
	REQUIRE(memcmp(read, data, 8) == 0);

	rs.RestoreRead();
	REQUIRE(rs.Read(read, 4));
	REQUIRE(memcmp(read, data, 4) == 0);

	REQUIRE(rs.Read(read, 4));
	REQUIRE(memcmp(read, data + 4, 4) == 0);

	rs.RestoreRead();

	REQUIRE(rs.Read(read, 4));
	REQUIRE(memcmp(read, data, 4) == 0);

	rs.CommitRead();

	REQUIRE(rs.Read(read, 4));
	REQUIRE(memcmp(read, data + 4, 4) == 0);

	rs.RestoreRead();

	REQUIRE(rs.Read(read, 4));
	REQUIRE(memcmp(read, data + 4, 4) == 0);

	rs.RestoreRead();

	std::shared_ptr<fisk::tools::StreamSegment> second = std::make_shared<fisk::tools::StreamSegment>();
	second->mySize									   = 8;
	memcpy(second->myData, data, 8);

	rs.AppendData(second);

	REQUIRE(!rs.Read(read, 16));

	rs.RestoreRead();

	REQUIRE(rs.Read(read, 8));
	REQUIRE(memcmp(read, data + 4, 4) == 0);
	REQUIRE(memcmp(read + 4, data, 4) == 0);

	rs.CommitRead();

	REQUIRE(rs.Read(read, 4));
	REQUIRE(memcmp(read, data + 4, 4) == 0);

	REQUIRE(!rs.Read(read, 4));

	rs.RestoreRead();
	REQUIRE(!rs.Read(read, 8));
	rs.CommitRead();

	std::shared_ptr<fisk::tools::StreamSegment> third = std::make_shared<fisk::tools::StreamSegment>();
	third->mySize									  = 8;
	memcpy(third->myData, data, 8);

	rs.AppendData(third);

	REQUIRE(rs.Read(read, 8));
	REQUIRE(memcmp(read, data, 8) == 0);

	rs.CommitRead();
}

TEST_CASE("StreamReader", "[Data]")
{
	fisk::tools::ReadStream rs;
	fisk::tools::StreamReader sr(rs);

	{
		std::shared_ptr<fisk::tools::StreamSegment> segment = std::make_shared<fisk::tools::StreamSegment>();

		const uint8_t data[] = {
			// clang-format off
			0b00000001,
			0b11111110,
			0b00000000, 0b00000011,
			0b11111111, 0b11111100,
			0b00000000, 0b00000000, 0b00000000, 0b00000101,
			0b11111111, 0b11111111, 0b11111111, 0b11111010,
			0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000111,
			0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111000,
			'H','e','l','l','o','\0'
			// clang-format on
		};

		segment->mySize = sizeof(data);
		memcpy(segment->myData, data, sizeof(data));

		rs.AppendData(segment);
	}

	uint8_t u8	 = 0;
	int8_t i8	 = 0;
	uint16_t u16 = 0;
	int16_t i16	 = 0;
	uint32_t u32 = 0;
	int32_t i32	 = 0;
	uint64_t u64 = 0;
	int64_t i64	 = 0;
	std::string s;

	REQUIRE(sr.Process(u8));
	REQUIRE(sr.Process(i8));
	REQUIRE(sr.Process(u16));
	REQUIRE(sr.Process(i16));
	REQUIRE(sr.Process(u32));
	REQUIRE(sr.Process(i32));
	REQUIRE(sr.Process(u64));
	REQUIRE(sr.Process(i64));
	REQUIRE(sr.Process(s));

	REQUIRE(u8 == 1);
	REQUIRE(i8 == -2);
	REQUIRE(u16 == 3);
	REQUIRE(i16 == -4);
	REQUIRE(u32 == 5);
	REQUIRE(i32 == -6);
	REQUIRE(u64 == 7);
	REQUIRE(i64 == -8);
	REQUIRE(s == "Hello");

	REQUIRE(!sr.Process(u8));
	REQUIRE(!sr.Process(i8));
	REQUIRE(!sr.Process(u16));
	REQUIRE(!sr.Process(i16));
	REQUIRE(!sr.Process(u32));
	REQUIRE(!sr.Process(i32));
	REQUIRE(!sr.Process(u64));
	REQUIRE(!sr.Process(i64));
	REQUIRE(!sr.Process(s));
}

TEST_CASE("JSON", "[Data]")
{

	REQUIRE(fisk::tools::JSONObject().Parse("{}"));
	{
		fisk::tools::JSONObject obj;
		REQUIRE(!obj);

		REQUIRE(obj.Parse("null"));
		REQUIRE(obj.IsNull());
		REQUIRE(!obj);

		REQUIRE(obj.Parse("{}"));
		REQUIRE(obj);
	}
	REQUIRE(!fisk::tools::JSONObject().Parse(""));
	REQUIRE(!fisk::tools::JSONObject().Parse("{"));
	REQUIRE(!fisk::tools::JSONObject().Parse("["));
	REQUIRE(!fisk::tools::JSONObject().Parse("{]"));
	REQUIRE(!fisk::tools::JSONObject().Parse("[\"x\":1 }"));
	REQUIRE(!fisk::tools::JSONObject().Parse("[1}"));
	REQUIRE(!fisk::tools::JSONObject().Parse("{\"x"));
	REQUIRE(!fisk::tools::JSONObject().Parse("{\"x\""));
	REQUIRE(!fisk::tools::JSONObject().Parse("{\"x\":"));
	REQUIRE(!fisk::tools::JSONObject().Parse("{\"x\":1"));
	REQUIRE(fisk::tools::JSONObject().Parse("{ }"));
	REQUIRE(fisk::tools::JSONObject().Parse("{\n}"));
	REQUIRE(fisk::tools::JSONObject().Parse("{\t}"));
	REQUIRE(fisk::tools::JSONObject().Parse("{\r}"));
	REQUIRE(fisk::tools::JSONObject().Parse("{\b}"));
	REQUIRE(fisk::tools::JSONObject().Parse("{\f}"));
	REQUIRE(fisk::tools::JSONObject().Parse("[ ]"));
	REQUIRE(fisk::tools::JSONObject().Parse("[\n]"));
	REQUIRE(fisk::tools::JSONObject().Parse("[\t]"));
	REQUIRE(fisk::tools::JSONObject().Parse("[\r]"));
	REQUIRE(fisk::tools::JSONObject().Parse("[\b]"));
	REQUIRE(fisk::tools::JSONObject().Parse("[\f]"));

	{
		fisk::tools::JSONObject root;
		{
			std::string s;
			int i = -1;
			long l		 = -1;
			long long ll = -1;
			size_t sz	 = 2;
			float f		 = 0.f;
			double d	 = 0.f;
			fisk::tools::JSONObject::ObjectType* obj;
			fisk::tools::JSONObject::ArrayType* arr;


			root.Parse("1");
			REQUIRE(root);
			REQUIRE(root.GetIf(i));
			REQUIRE(!root.GetIf(s));
			REQUIRE(i == 1);
			REQUIRE(root.GetIf(l));
			REQUIRE(l == 1);
			REQUIRE(root.GetIf(ll));
			REQUIRE(ll == 1);
			REQUIRE(root.GetIf(sz));
			REQUIRE(sz == 1);

			REQUIRE(root.Parse("{\"x\":2}"));
			REQUIRE(root);
			REQUIRE(root["x"]);
			REQUIRE(root["x"].GetIf(i));
			REQUIRE(i == 2);

			i = -1;

			REQUIRE(root.GetIf(obj));
			REQUIRE(obj);
			REQUIRE(obj->size() == 1);
			REQUIRE(obj->count("x") == 1);
			REQUIRE(obj->at("x").get() == &root["x"]);

			using namespace std::string_view_literals;
			REQUIRE(root.Parse("\"hello\""));
			REQUIRE(root);
			REQUIRE(root.GetIf(s));
			REQUIRE(s == "hello");

			REQUIRE(root.Parse("1.0"));
			REQUIRE(root);
			REQUIRE(root.GetIf(f));
			REQUIRE(f == Catch::Approx(1.0f));
			REQUIRE(root.GetIf(d));
			REQUIRE(!root.GetIf(s));
			REQUIRE(d == Catch::Approx(1.0));

			REQUIRE(root.Parse("[10]"));

			REQUIRE(root[0]);
			REQUIRE(root[0].GetIf(i));
			REQUIRE(i == 10);

			REQUIRE(root.GetIf(arr));
			REQUIRE(arr);
			REQUIRE(arr->size() == 1);
			REQUIRE(arr->at(0));
			REQUIRE(arr->at(0).get() == &root[0]);
		
			REQUIRE(root.Parse(R"({
									"0": 0,	
									"1": 1,
									"2": 2, 
									"3": 3, 
									"4": 4 
								})"));


			bool seen[5] = {false, false, false, false, false};
			for (const auto& [key, value] : root.IterateObject())
			{
				REQUIRE(value.GetIf(i));
				REQUIRE(std::to_string(i) == key);
				REQUIRE(i < 6);
				seen[i] = true;
			}

			REQUIRE(seen[0]);
			REQUIRE(seen[1]);
			REQUIRE(seen[2]);
			REQUIRE(seen[3]);
			REQUIRE(seen[4]);
		}
	}
}
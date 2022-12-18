

#include <catch2/catch_all.hpp>

#include "tools/StreamReader.h"
#include "tools/StreamWriter.h"

#include <algorithm>

TEST_CASE("Object pool", "[Container]")
{
	{
		fisk::tools::ObjectPool<int> pool(16);

		int* a = pool.GetItem();
		REQUIRE(a);
		*a = 5;

		REQUIRE(*a == 5);

		int* b = pool.GetItem(3);
		REQUIRE(b);
		REQUIRE(*b == 3);
		REQUIRE(a != b);

		*b = 6;
		REQUIRE(*b == 6);
		REQUIRE(*a != 6);

		pool.ReturnItem(a);
		int* c = pool.GetItem();

		REQUIRE(c == a);

		pool.ReturnItem(b);
		pool.ReturnItem(c);
	}

	{
		fisk::tools::ObjectPool<int> pool2(2, false);

		int* d = pool2.GetItem();
		int* e = pool2.GetItem();

		REQUIRE(!pool2.GetItem());

		pool2.ReturnItem(d);

		int* f = pool2.GetItem();

		REQUIRE(f);
		REQUIRE(!pool2.GetItem());

		pool2.ReturnItem(e);
		pool2.ReturnItem(f);
	}
}

TEST_CASE("WriteStream", "[Data]")
{
	SECTION("Basic")
	{
		fisk::tools::WriteStream ws;

		REQUIRE(!ws.Get());

		const uint8_t data[8]{1, 2, 3, 4, 5, 6, 7, 8};

		ws.WriteData(data, 8);

		std::shared_ptr<fisk::tools::StreamSegment> streamData = ws.Get();

		REQUIRE(streamData);
		REQUIRE(!streamData->myNext);
		REQUIRE(streamData->mySize == 8);
		REQUIRE(memcmp(streamData->myData, data, 8) == 0);

		ws.WriteData(data, 8);

		streamData = ws.Get();

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

	REQUIRE(sw.Process(u8));
	REQUIRE(sw.Process(i8));
	REQUIRE(sw.Process(u16));
	REQUIRE(sw.Process(i16));
	REQUIRE(sw.Process(u32));
	REQUIRE(sw.Process(i32));
	REQUIRE(sw.Process(u64));
	REQUIRE(sw.Process(i64));

	std::shared_ptr<fisk::tools::StreamSegment> head = ws.Get();

	const uint8_t expected[] = {
		// clang-format off
		0b00000001,
		0b11111110,
		0b00000011, 0b00000000,
		0b11111100, 0b11111111,
		0b00000101, 0b00000000, 0b00000000, 0b00000000,
		0b11111010, 0b11111111, 0b11111111, 0b11111111,
		0b00000111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
		0b11111000, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111
		// clang-format on
	};

	REQUIRE(!head->myNext);
	REQUIRE(head->mySize == 30);
	REQUIRE(memcmp(head->myData, expected, 30) == 0);
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
			0b00000011, 0b00000000,
			0b11111100, 0b11111111,
			0b00000101, 0b00000000, 0b00000000, 0b00000000,
			0b11111010, 0b11111111, 0b11111111, 0b11111111,
			0b00000111, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000, 0b00000000,
			0b11111000, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111
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

	REQUIRE(sr.Process(u8));
	REQUIRE(sr.Process(i8));
	REQUIRE(sr.Process(u16));
	REQUIRE(sr.Process(i16));
	REQUIRE(sr.Process(u32));
	REQUIRE(sr.Process(i32));
	REQUIRE(sr.Process(u64));
	REQUIRE(sr.Process(i64));

	REQUIRE(u8 == 1);
	REQUIRE(i8 == -2);
	REQUIRE(u16 == 3);
	REQUIRE(i16 == -4);
	REQUIRE(u32 == 5);
	REQUIRE(i32 == -6);
	REQUIRE(u64 == 7);
	REQUIRE(i64 == -8);

	REQUIRE(!sr.Process(u8));
	REQUIRE(!sr.Process(i8));
	REQUIRE(!sr.Process(u16));
	REQUIRE(!sr.Process(i16));
	REQUIRE(!sr.Process(u32));
	REQUIRE(!sr.Process(i32));
	REQUIRE(!sr.Process(u64));
	REQUIRE(!sr.Process(i64));
}
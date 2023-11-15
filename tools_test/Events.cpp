
#include <catch2/catch_all.hpp>

#include "tools/Event.h"

TEST_CASE("Event", "[Events]")
{
	{
		fisk::tools::Event<> ev;

		bool fired = false;

		ev.Fire();

		REQUIRE(!fired);

		fisk::tools::EventReg reg = ev.Register([&fired]() { fired = true; });

		REQUIRE(!fired);

		ev.Fire();

		REQUIRE(fired);
	}

	{
		fisk::tools::Event<int> ev;

		int value = -1;

		ev.Fire(1);

		REQUIRE(value == -1);

		fisk::tools::EventReg reg = ev.Register([&value](int aValue) { value = aValue; });

		REQUIRE(value == -1);

		ev.Fire(1);

		REQUIRE(value == 1);
	}

	{
		fisk::tools::Event<int> ev;

		int value = -1;

		ev.Fire(1);

		REQUIRE(value == -1);

		fisk::tools::EventReg reg = ev.Register([&value](int aValue) { value = aValue; });

		REQUIRE(value == -1);

		ev.Fire(1);

		REQUIRE(value == 1);
		
		reg = ev.Register([&value](int aValue) { value = aValue + 5; });
		
		REQUIRE(value == 1);
		
		ev.Fire(1);
		
		REQUIRE(value == 6);
	}

	{
		fisk::tools::ShortCircutableEvent<int> ev;

		REQUIRE(!ev.Fire());

		fisk::tools::EventReg reg1 = ev.Register([]() { return std::optional<int>(); });
		fisk::tools::EventReg reg2 = ev.Register([]() { return std::optional<int>(); });

		REQUIRE(!ev.Fire());

		fisk::tools::EventReg reg3 = ev.Register([]() { return std::optional<int>(5); });

		REQUIRE(ev.Fire());
		REQUIRE(*ev.Fire() == 5);

		fisk::tools::EventReg reg4 = ev.Register([]() { return std::optional<int>(6); });

		REQUIRE(ev.Fire());
		REQUIRE(*ev.Fire() == 5);

		reg2 = nullptr;

		REQUIRE(ev.Fire());
		REQUIRE(*ev.Fire() == 5);

		reg3 = nullptr;

		REQUIRE(ev.Fire());
		REQUIRE(*ev.Fire() == 6);

		reg1 = nullptr;

		REQUIRE(ev.Fire());
		REQUIRE(*ev.Fire() == 6);

		reg4 = nullptr;

		REQUIRE(!ev.Fire());

	}
}
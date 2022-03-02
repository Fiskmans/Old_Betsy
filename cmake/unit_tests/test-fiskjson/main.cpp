
#include <ranges>

#include "catch2/catch_all.hpp"

#include "tools/FiskJSON.h"
#include "tools/FloatCompare.h"


TEST_CASE("Fiskjson ZOMBIES", "[factorial]") {

//Z

	SECTION("Z")
	{
		REQUIRE(Tools::FiskJSON::Object().Parse("{}"));
		REQUIRE(Tools::FiskJSON::Object().Parse("null").IsNull());
		REQUIRE(!Tools::FiskJSON::Object().Parse("null"));
		REQUIRE_THROWS_AS(Tools::FiskJSON::Object().Parse(""), Tools::FiskJSON::Invalid_JSON);
		REQUIRE_THROWS_AS(Tools::FiskJSON::Object().Parse("{"), Tools::FiskJSON::Invalid_JSON);
		REQUIRE_THROWS_AS(Tools::FiskJSON::Object().Parse("["), Tools::FiskJSON::Invalid_JSON);
		REQUIRE_THROWS_AS(Tools::FiskJSON::Object().Parse("{]"), Tools::FiskJSON::Invalid_JSON);
		REQUIRE_THROWS_AS(Tools::FiskJSON::Object().Parse("[\"x\":1 }"), Tools::FiskJSON::Invalid_JSON);
		REQUIRE_THROWS_AS(Tools::FiskJSON::Object().Parse("[1}"), Tools::FiskJSON::Invalid_JSON);
		REQUIRE_THROWS_AS(Tools::FiskJSON::Object().Parse("{\"x"), Tools::FiskJSON::Invalid_JSON);
		REQUIRE_THROWS_AS(Tools::FiskJSON::Object().Parse("{\"x\""), Tools::FiskJSON::Invalid_JSON);
		REQUIRE_THROWS_AS(Tools::FiskJSON::Object().Parse("{\"x\":"), Tools::FiskJSON::Invalid_JSON);
		REQUIRE_THROWS_AS(Tools::FiskJSON::Object().Parse("{\"x\":1"), Tools::FiskJSON::Invalid_JSON);
		REQUIRE(Tools::FiskJSON::Object().Parse("{ }"));
		REQUIRE(Tools::FiskJSON::Object().Parse("{\n}"));
		REQUIRE(Tools::FiskJSON::Object().Parse("{\t}"));
		REQUIRE(Tools::FiskJSON::Object().Parse("{\r}"));
		REQUIRE(Tools::FiskJSON::Object().Parse("{\b}"));
		REQUIRE(Tools::FiskJSON::Object().Parse("{\f}"));
		REQUIRE(Tools::FiskJSON::Object().Parse("[ ]"));
		REQUIRE(Tools::FiskJSON::Object().Parse("[\n]"));
		REQUIRE(Tools::FiskJSON::Object().Parse("[\t]"));
		REQUIRE(Tools::FiskJSON::Object().Parse("[\r]"));
		REQUIRE(Tools::FiskJSON::Object().Parse("[\b]"));
		REQUIRE(Tools::FiskJSON::Object().Parse("[\f]"));
	}

	SECTION("O")
	{
		Tools::FiskJSON::Object root;
		SECTION("int")
		{
			root.Parse("1");
			REQUIRE(root);
			REQUIRE(root.Is<int>());
			REQUIRE(root.Get<int>() == 1);
			REQUIRE(root.Is<long>());
			REQUIRE(root.Get<long>() == 1);
			REQUIRE(root.Is<long long>());
			REQUIRE(root.Get<long long>() == 1);
			REQUIRE(root.Is<size_t>());
			REQUIRE(root.Get<size_t>() == 1);

			root.Parse("{\"x\":2}");
			REQUIRE(root);
			REQUIRE(root["x"]);
			REQUIRE(root["x"].Is<int>());
			REQUIRE(root["x"].Get<int>() == 2);
		}

		SECTION("string")
		{
			using namespace std::string_view_literals;
			root.Parse("\"hello\"");
			REQUIRE(root);
			REQUIRE(root.Is<std::string>());
			REQUIRE(root.Get<std::string>() == "hello");
			REQUIRE(root.Is<const char*>());
			REQUIRE(root.Get<const char*>() == "hello"sv);
			REQUIRE(root.Is<std::string>());
			REQUIRE(root.Get<std::string>() == "hello");
		}

		SECTION("floating point")
		{
			root.Parse("1.0");
			REQUIRE(root);
			REQUIRE(root.Is<float>());
			REQUIRE(root.Get<float>() == Tools::FloatCompare(1.0f, 1.e-8f));
			REQUIRE(root.Is<double>());
			REQUIRE(root.Get<double>() == Tools::FloatCompare(1.0, 1.e-8));
		}
	}

}
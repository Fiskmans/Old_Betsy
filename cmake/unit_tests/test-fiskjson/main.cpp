
#include <ranges>

#include "catch2/catch_all.hpp"

#include "tools/FiskJSON.h"


TEST_CASE("Factorials are computed", "[factorial]") {

	Tools::FiskJSON::Object root;
	root.Parse("{}");

	REQUIRE(root.NotNull());
}
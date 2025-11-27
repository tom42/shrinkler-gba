// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <sstream>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

TEST_CASE("table_printer")
{
	std::stringstream stream;
	shrinkler_gba::console console;
	shrinkler_gba::table_printer printer;

	SECTION("print, empty table")
	{
		printer.print(console);

		CHECK(stream.str() == "");
	}
}

}

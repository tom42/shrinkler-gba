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

	console.verbose_stream(&stream);

	SECTION("print empty table")
	{
		printer.print(console);

		CHECK(stream.str() == "");
	}

	SECTION("print one row")
	{
		printer.add_row({ "a", "bc", "def"});

		printer.print(console);

		CHECK(stream.str() == "a bc def\n");
	}

	SECTION("print two rows with same widths")
	{
		printer.add_row({ "a", "bc", "def" });
		printer.add_row({ "1", "23", "456" });

		printer.print(console);

		CHECK(stream.str() ==
			"a bc def\n"
			"1 23 456\n");
	}

	SECTION("print rows that need aligning")
	{
		printer.add_row({ "a", "bcd", "ef" });
		printer.add_row({ "12", "2", "456", "7"});

		printer.print(console);

		CHECK(stream.str() ==
			"a  bcd ef\n"
			"12 2   456 7\n");
	}

	SECTION("print with indentation")
	{
		printer.table_indent(5);
		printer.add_row({ "ab", "c" });
		printer.add_row({ "1", "23" });

		printer.print(console);

		CHECK(stream.str() ==
			"     ab c\n"
			"     1  23\n");
	}
}

}

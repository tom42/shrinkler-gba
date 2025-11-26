// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::to_hex;

TEST_CASE("elf_strings")
{
	SECTION("to_hex")
	{
		CHECK(to_hex(0xfu) == "0xf");
		CHECK(to_hex(0xffu) == "0xff");
		CHECK(to_hex(0xfffu) == "0xfff");
	}
}

}

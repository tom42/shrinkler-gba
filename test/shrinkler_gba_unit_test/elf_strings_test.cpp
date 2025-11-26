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
		CHECK(to_hex(0xau) == "0xa");
		CHECK(to_hex(0xabu) == "0xab");
		CHECK(to_hex(0xabcu) == "0xabc");
	}
}

}

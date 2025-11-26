// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::to_hex;

TEST_CASE("elf_strings")
{
    SECTION("to_hex, without width specifier")
    {
        CHECK(to_hex(0xau) == "0xa");
        CHECK(to_hex(0xabu) == "0xab");
        CHECK(to_hex(0xabcu) == "0xabc");
    }

    SECTION("to_hex, with width specifier")
    {
        CHECK(to_hex(0xau, 2) == "0x0a");
        CHECK(to_hex(0xabu, 2) == "0xab");
        CHECK(to_hex(0xabcu, 2) == "0xabc");
        CHECK(to_hex(0xabcu, 4) == "0x0abc");
    }
}

}

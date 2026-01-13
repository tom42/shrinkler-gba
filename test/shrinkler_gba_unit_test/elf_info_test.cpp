// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <elfio/elfio.hpp>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using namespace ELFIO;
using shrinkler_gba::get_section_flags;
using shrinkler_gba::get_segment_flags;
using shrinkler_gba::to_hex;

TEST_CASE("elf_info")
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

    SECTION("get_section_flags, known flags")
    {
        CHECK(get_section_flags(SHF_WRITE) == "W");
        CHECK(get_section_flags(SHF_ALLOC) == "A");
        CHECK(get_section_flags(SHF_WRITE | SHF_EXECINSTR) == "WX");
    }

    SECTION("get_section_flags, known and unknown flags")
    {
        CHECK(get_section_flags(SHF_WRITE | 0x80000000) == "0x80000001");
    }

    SECTION("get_segment_flags")
    {
        // No full coverage here
        CHECK(get_segment_flags(0) == "");
        CHECK(get_segment_flags(PF_R) == "R");
        CHECK(get_segment_flags(PF_R | PF_W | PF_X) == "RWX");
    }
}

}

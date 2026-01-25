// SPDX-FileCopyrightText: 2026 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <string>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using namespace std::string_literals;
namespace packer_registry = shrinkler_gba::packer_registry;

TEST_CASE("packer_registry")
{
    SECTION("find_info")
    {
        CHECK(packer_registry::find_info("lzss")->name == "lzss"s);
        CHECK(packer_registry::find_info("shrinkler")->name == "shrinkler"s);
        CHECK(packer_registry::find_info("does_not_exist") == nullptr);
    }
}

}

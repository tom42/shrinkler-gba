// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <elfio/elfio.hpp>
#include <filesystem>
#include <stdexcept>
#include <string_view>
#include "shrinkler_gba_unit_test_config.hpp"
#include "test_utilities.hpp"

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::input_file;

namespace
{

input_file create_input_file(std::string_view filename)
{
    auto path = std::filesystem::path(SHRINKLER_GBA_UNIT_TEST_TESTDATA_DIRECTORY) / filename;

    ELFIO::elfio elfio;
    if (!elfio.load(path.string()))
    {
        throw std::runtime_error("Could not load test ELF file");
    }

    return input_file(elfio);
}

}

TEST_CASE("input_file")
{
    SECTION("constructor, valid file, ARM entry")
    {
        auto input_file = create_input_file("lostmarbles.elf");

        CHECK(input_file.entry() == 0x03000000);
        CHECK(input_file.is_thumb_entry() == false);
        CHECK(input_file.load_address() == 0x03000000);
        CHECK(input_file.data() == load_binary_file("lostmarbles.bin"));
        CHECK(input_file.data().size() == input_file.loaded_data_size());
    }

    SECTION("constructor, valid file, Thumb entry")
    {
        auto input_file = create_input_file("thumb_entry.elf");

        CHECK(input_file.entry() == 0x8001);
        CHECK(input_file.is_thumb_entry() == true);
        CHECK(input_file.load_address() == 0x8000);
        CHECK(input_file.data() == load_binary_file("thumb_entry.bin"));
        CHECK(input_file.data().size() == input_file.loaded_data_size());
    }
}

}

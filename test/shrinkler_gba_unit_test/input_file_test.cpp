// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>
#include "shrinkler_gba_unit_test_config.hpp"
#include "test_utilities.hpp"

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using shrinkler_gba::input_file;

namespace
{

// TODO: see what to do about this
/*
input_file load(const std::string& filename)
{
    auto path = std::filesystem::path(SHRINKLER_GBA_UNIT_TEST_TESTDATA_DIRECTORY) / filename;

    shrinkler_gba::console console;
    console.out_stream(nullptr);
    console.warn_stream(nullptr);
    console.verbose_stream(nullptr);

    return input_file::load(path.string(), console);
}*/

}

TEST_CASE("input_file")
{
    // TODO: see what to do about these tests
    /*
    SECTION("load, valid file, ARM entry")
    {
        auto input_file = load("lostmarbles.elf");

        CHECK(input_file.entry() == 0x03000000);
        CHECK(input_file.is_thumb_entry() == false);
        CHECK(input_file.load_address() == 0x03000000);
        CHECK(input_file.data() == load_binary_file("lostmarbles.bin"));
        CHECK(input_file.data().size() == input_file.loaded_data_size());
    }

    SECTION("load, valid file, Thumb entry")
    {
        auto input_file = load("thumb_entry.elf");

        CHECK(input_file.entry() == 0x8001);
        CHECK(input_file.is_thumb_entry() == true);
        CHECK(input_file.load_address() == 0x8000);
        CHECK(input_file.data() == load_binary_file("thumb_entry.bin"));
        CHECK(input_file.data().size() == input_file.loaded_data_size());
    }*/
}

}

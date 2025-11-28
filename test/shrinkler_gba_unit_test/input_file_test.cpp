// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>
#include "shrinkler_gba_unit_test_config.hpp"
#include "test_utilities.hpp"

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using Catch::CaseSensitive;
using Catch::Matchers::MessageMatches;
using Catch::Matchers::EndsWith;

class input_file_fixture
{
protected:
    void load(const std::string& filename)
    {
        auto path = std::filesystem::path(SHRINKLER_GBA_UNIT_TEST_TESTDATA_DIRECTORY) / filename;

        shrinkler_gba::console console;
        console.out_stream(nullptr);
        console.warn_stream(nullptr);
        console.verbose_stream(nullptr);

        input_file.load_old(path.string(), console);
    }

    shrinkler_gba::input_file input_file;
};

TEST_CASE_METHOD(input_file_fixture, "input_file")
{
    SECTION("state after construction")
    {
        CHECK(input_file.entry() == 0);
        CHECK(input_file.load_address() == 0);
        CHECK(input_file.loaded_data_size() == 0);
        CHECK(input_file.data().size() == 0);
    }

    SECTION("load, file does not exist")
    {
        CHECK_THROWS_MATCHES(
            load("non-existing-file.elf"),
            std::runtime_error,
            MessageMatches(EndsWith("non-existing-file.elf: no such file or directory", CaseSensitive::No)));
    }

    SECTION("load, invalid file")
    {
        CHECK_THROWS_MATCHES(
            load("invalid-elf-file.elf"),
            std::runtime_error,
            MessageMatches(EndsWith("invalid-elf-file.elf: file is not a valid ELF file", CaseSensitive::No)));
    }

    SECTION("load, valid file, ARM entry")
    {
        load("lostmarbles.elf");

        CHECK(input_file.entry() == 0x03000000);
        CHECK(input_file.is_thumb_entry() == false);
        CHECK(input_file.load_address() == 0x03000000);
        CHECK(input_file.data() == load_binary_file("lostmarbles.bin"));
        CHECK(input_file.data().size() == input_file.loaded_data_size());
    }

    SECTION("load, valid file, Thumb entry")
    {
        load("thumb_entry.elf");

        CHECK(input_file.entry() == 0x8001);
        CHECK(input_file.is_thumb_entry() == true);
        CHECK(input_file.load_address() == 0x8000);
        CHECK(input_file.data() == load_binary_file("thumb_entry.bin"));
        CHECK(input_file.data().size() == input_file.loaded_data_size());
    }
}

}

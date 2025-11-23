// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <filesystem>
#include <stdexcept>
#include <string>
#include "shrinkler_gba_unit_test_config.hpp"

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

class input_file_fixture
{
protected:
    void load(const std::string& filename)
    {
        auto path = std::filesystem::path(SHRINKLER_GBA_UNIT_TEST_TESTDATA_DIRECTORY) / filename;
        input_file.load(path.string());
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
            Catch::Matchers::Message("non-existing-file.elf: no such file or directory"));
    }

    SECTION("load, invalid file")
    {
        CHECK_THROWS_MATCHES(
            load("invalid-elf-file.elf"),
            std::runtime_error,
            Catch::Matchers::Message("invalid-elf-file.elf: file is not a valid ELF file"));
    }
}

}

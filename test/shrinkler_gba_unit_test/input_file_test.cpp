// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <stdexcept>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

TEST_CASE("input_file")
{
    shrinkler_gba::input_file input_file;

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
            input_file.load("non-existing-file.elf"),
            std::runtime_error,
            Catch::Matchers::Message("non-existing-file.elf: no such file or directory"));
    }
}

}

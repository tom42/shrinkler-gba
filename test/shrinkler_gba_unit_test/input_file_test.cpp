// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>

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
        // TODO: expect some sort of exception, and its message
        input_file.load("non-existing-file.elf");
    }
}

}

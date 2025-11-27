// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <sstream>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

TEST_CASE("console")
{
    std::stringstream out_stream;
    std::stringstream warn_stream;
    std::stringstream verbose_stream;

    shrinkler_gba::console console;
    console.out_stream(&out_stream);
    console.warn_stream(&warn_stream);
    console.verbose_stream(&verbose_stream);

    SECTION("out, output is disabled")
    {
        console.out_stream(nullptr);

        console.out("log message");

        CHECK(out_stream.str() == "");
        CHECK(warn_stream.str() == "");
        CHECK(verbose_stream.str() == "");
    }

    SECTION("out, output is enabled")
    {
        console.out("log message");

        CHECK(out_stream.str() == "log message\n");
        CHECK(warn_stream.str() == "");
        CHECK(verbose_stream.str() == "");
    }
}

}

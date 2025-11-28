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

        console.out("formatted: {}", 42);

        CHECK(out_stream.str() == "");
        CHECK(warn_stream.str() == "");
        CHECK(verbose_stream.str() == "");
    }

    SECTION("out, output is enabled")
    {
        console.out("formatted: {}", 42);

        CHECK(out_stream.str() == "formatted: 42\n");
        CHECK(warn_stream.str() == "");
        CHECK(verbose_stream.str() == "");
    }

    SECTION("warn, output is disabled")
    {
        console.warn_stream(nullptr);

        console.warn("formatted: {}", 42);

        CHECK(out_stream.str() == "");
        CHECK(warn_stream.str() == "");
        CHECK(verbose_stream.str() == "");
    }

    SECTION("warn, output is enabled")
    {
        console.warn("formatted: {}", 42);

        CHECK(out_stream.str() == "");
        CHECK(warn_stream.str() == "Warning: formatted: 42\n");
        CHECK(verbose_stream.str() == "");
    }

    SECTION("verbose, output is disabled")
    {
        console.verbose_stream(nullptr);

        console.verbose("const char*");
        console.verbose(nullptr);
        console.verbose(std::string("const std::string&"));
        console.verbose("formatted: {}", 42);

        CHECK(out_stream.str() == "");
        CHECK(warn_stream.str() == "");
        CHECK(verbose_stream.str() == "");
    }

    SECTION("verbose, output is enabled")
    {
        console.verbose("const char*");
        console.verbose(nullptr);
        console.verbose(std::string("const std::string&"));
        console.verbose("formatted: {}", 42);

        CHECK(out_stream.str() == "");
        CHECK(warn_stream.str() == "");
        CHECK(verbose_stream.str() ==
            "const char*\n"
            "(null)\n"
            "const std::string&\n"
            "formatted: 42\n");
    }

    SECTION("is_verbose_enabled")
    {
        console.verbose_stream(nullptr);
        CHECK(console.is_verbose_enabled() == false);

        console.verbose_stream(&verbose_stream);
        CHECK(console.is_verbose_enabled() == true);
    }
}

}

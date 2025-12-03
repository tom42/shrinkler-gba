// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <filesystem>
#include "shrinkler_gba_unit_test_config.hpp"

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using Catch::CaseSensitive;
using Catch::Matchers::MessageMatches;
using Catch::Matchers::EndsWith;
using shrinkler_gba::options;
using shrinkler_gba::pack;

namespace
{

class pack_fixture
{
protected:
    void set_input_file(const char* filename)
    {
        opts.input_file(std::filesystem::path(SHRINKLER_GBA_UNIT_TEST_TESTDATA_DIRECTORY) / filename);
    }

    options opts;
};

}

TEST_CASE_METHOD(pack_fixture, "pack")
{
    SECTION("input file does not exist")
    {
        set_input_file("non-existing-file.elf");

        // The error message 'no such file or directory' is supplied by the standard library.
        // Case varies between libraries, so we use case insensitive comparison.
        CHECK_THROWS_MATCHES(
            pack(opts),
            std::runtime_error,
            MessageMatches(EndsWith("non-existing-file.elf: no such file or directory", CaseSensitive::No)));
    }

    SECTION("input file is not a valid ELF file")
    {
        set_input_file("invalid-elf-file.elf");

        CHECK_THROWS_MATCHES(
            pack(opts),
            std::runtime_error,
            MessageMatches(EndsWith("invalid-elf-file.elf: File is not a valid ELF file", CaseSensitive::Yes)));
    }
}

}

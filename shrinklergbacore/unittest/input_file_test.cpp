// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <boost/algorithm/string/predicate.hpp>
#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <stdexcept>
#include "shrinklergbacore/input_file.hpp"
#include "shrinklergbacore_unittest_config.hpp"
#include "test_utilities.hpp"

namespace shrinklergbacore_unittest
{

using std::runtime_error;
using shrinklergbacore::input_file;

static shrinklergbacore::console create_console(bool verbose)
{
    shrinklergbacore::console c;

    if (!verbose)
    {
        c.verbose(nullptr);
        c.out(nullptr);
    }

    return c;
}

static input_file load_elf_file(const std::filesystem::path& filename, bool verbose = false)
{
    input_file f(create_console(verbose));
    f.load(SHRINKLERGBACORE_UNITTEST_TESTDATA_DIRECTORY / filename);
    return f;
}

BOOST_AUTO_TEST_SUITE(input_file_test)

    BOOST_AUTO_TEST_CASE(constructor)
    {
        const input_file testee;

        BOOST_TEST(testee.entry() == 0u);
        BOOST_TEST(testee.load_address() == 0u);
        BOOST_TEST(testee.loaded_data_size() == 0u);
        BOOST_TEST(testee.data().size() == 0u);
    }

    BOOST_AUTO_TEST_CASE(load_when_file_does_not_exist_then_throws)
    {
        BOOST_CHECK_EXCEPTION(
            load_elf_file("non-existing-file.elf"),
            runtime_error,
            [](const auto& e) { return boost::iends_with(e.what(), "non-existing-file.elf: no such file or directory"); });
    }

    BOOST_AUTO_TEST_CASE(load_when_elf_file_is_invalid_then_throws)
    {
        BOOST_CHECK_EXCEPTION(
            load_elf_file("invalid-elf-file.elf"),
            runtime_error,
            [](const auto& e) { return boost::iends_with(e.what(), "file is not a valid ELF file"); });
    }

    BOOST_AUTO_TEST_CASE(load)
    {
        auto testee = load_elf_file("lostmarbles.elf");

        BOOST_TEST(testee.entry() == 0x03000000u);
        BOOST_TEST(testee.is_thumb_entry() == false);
        BOOST_TEST(testee.load_address() == 0x03000000u);
        BOOST_TEST(testee.data() == load_binary_file("lostmarbles.bin"), boost::test_tools::per_element());
        BOOST_TEST(testee.data().size() == testee.loaded_data_size());
    }

    BOOST_AUTO_TEST_CASE(load_thumb_entry)
    {
        auto testee = load_elf_file("thumb_entry.elf");

        BOOST_TEST(testee.entry() == 0x8001u);
        BOOST_TEST(testee.is_thumb_entry() == true);
        BOOST_TEST(testee.load_address() == 0x8000u);
        BOOST_TEST(testee.data() == load_binary_file("thumb_entry.bin"), boost::test_tools::per_element());
        BOOST_TEST(testee.data().size() == testee.loaded_data_size());
    }

BOOST_AUTO_TEST_SUITE_END()

}

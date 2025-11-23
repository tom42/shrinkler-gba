#include <boost/algorithm/string/predicate.hpp>
#include <boost/test/unit_test.hpp>
#include <filesystem>
#include <stdexcept>
#include "shrinklergbacore/input_file.hpp"
#include "shrinklergbacore_unittest_config.hpp"
#include "test_utilities.hpp"

namespace shrinklergbacore_unittest
{

static input_file load_elf_file(const std::filesystem::path& filename, bool verbose = false)
{
    input_file f(create_console(verbose));
    f.load(SHRINKLERGBACORE_UNITTEST_TESTDATA_DIRECTORY / filename);
    return f;
}

BOOST_AUTO_TEST_SUITE(input_file_test)

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

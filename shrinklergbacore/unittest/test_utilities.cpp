// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <boost/numeric/conversion/cast.hpp>
#include <boost/test/unit_test.hpp>
#include <cstddef>
#include <fstream>
#include <iterator>
#include "shrinklergbacore_unittest_config.hpp"
#include "test_utilities.hpp"

namespace shrinklergbacore_unittest
{

using std::filesystem::path;
using std::vector;

std::vector<unsigned char> load_binary_file(const std::filesystem::path& filename)
{
    path full_path = SHRINKLERGBACORE_UNITTEST_TESTDATA_DIRECTORY / filename;

    // Open file, stop it from eating whitespace.
    std::ifstream file(full_path, std::ios::binary);
    BOOST_REQUIRE_MESSAGE(file, "Could not open " + full_path.string());
    file.unsetf(std::ios::skipws);

    // Create vector with sufficient capacity to hold entire file.
    auto filesize = std::filesystem::file_size(full_path);
    vector<unsigned char> data;
    data.reserve(boost::numeric_cast<size_t>(filesize));

    // Read entire file
    data.insert(
        data.begin(),
        std::istream_iterator<unsigned char>(file),
        std::istream_iterator<unsigned char>());
    BOOST_REQUIRE_MESSAGE(file || file.eof(), "Error reading " + full_path.string());
    file.close();

    return data;
}

const std::vector<unsigned char> make_bytevector(const char* s)
{
    return std::vector<unsigned char>(s, s + std::strlen(s));
}

}

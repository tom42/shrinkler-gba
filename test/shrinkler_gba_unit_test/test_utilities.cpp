// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <gsl/gsl>
#include <iterator>
#include <stdexcept>
#include "shrinkler_gba_unit_test_config.hpp"
#include "test_utilities.hpp"

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

using std::filesystem::path;
using std::vector;

shrinkler_gba::bytevector load_binary_file(const std::string& filename)
{
    path full_path = std::filesystem::path(SHRINKLER_GBA_UNIT_TEST_TESTDATA_DIRECTORY) / filename;

    // Open file
    std::ifstream file(full_path, std::ios::binary);
    if (!file)
    {
        throw std::runtime_error("Could not open " + full_path.string());
    }

    // Stop file from eating whitespace.
    file.unsetf(std::ios::skipws);

    // Create vector with sufficient capacity to hold entire file.
    auto filesize = std::filesystem::file_size(full_path);
    vector<unsigned char> data;
    data.reserve(gsl::narrow<size_t>(filesize));

    // Read entire file
    data.insert(
        data.begin(),
        std::istream_iterator<unsigned char>(file),
        std::istream_iterator<unsigned char>());
    if (file.bad() || (data.size() != filesize))
    {
        throw std::runtime_error("Error reading " + full_path.string());
    }

    file.close();
    return data;
}

shrinkler_gba::bytevector make_bytevector(const char* s)
{
    return shrinkler_gba::bytevector(s, s + std::strlen(s));
}

}

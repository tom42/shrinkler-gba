// MIT License
//
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
// Copyright (c) 2021 Thomas Mathys
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <boost/numeric/conversion/cast.hpp>
#include <boost/test/unit_test.hpp>
#include <cstddef>
#include <fstream>
#include "shrinklergba_unittest_config.hpp"
#include "test_utilities.hpp"

namespace shrinklergba_unittest
{

using std::filesystem::path;
using std::vector;

std::vector<unsigned char> load_binary_file(const std::filesystem::path& filename)
{
    path full_path = SHRINKLERGBA_UNITTEST_TESTDATA_DIRECTORY / filename;

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

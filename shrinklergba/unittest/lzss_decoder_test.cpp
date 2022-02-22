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

#include <boost/test/unit_test.hpp>
#include "shrinklergba/lzss.hpp"
#include "test_utilities.hpp"

namespace shrinklergba_unittest
{

class lzss_decoder_test_fixture
{
public:
    shrinklergba::lzss_decoder decoder;
};

BOOST_FIXTURE_TEST_SUITE(lzss_decoder_test, lzss_decoder_test_fixture)

// TODO: tests:
//       * compressed data size (alignment?)
//       * reserved bits in header must be 0 (orly?)
//       * compression type (that one would be a good start)
//       * Actually decompress something

    BOOST_AUTO_TEST_CASE(decode_when_compressed_data_is_too_short_then_throws)
    {
        std::vector<unsigned char> data{ 0x10, 0x00, 0x00 };
        CHECK_EXCEPTION(decoder.decode(data), std::runtime_error, "compressed data is too short");
    }

    BOOST_AUTO_TEST_CASE(decode_when_compression_type_is_wrong_then_throws)
    {
        std::vector<unsigned char> data{ 0x00, 0x00, 0x00, 0x00 };
        CHECK_EXCEPTION(decoder.decode(data), std::runtime_error, "invalid compression type");
    }

BOOST_AUTO_TEST_SUITE_END()

}

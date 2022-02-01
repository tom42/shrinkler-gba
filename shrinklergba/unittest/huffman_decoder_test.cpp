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
#include <vector>
#include "shrinklergba/huffman.hpp"

namespace shrinklergba_unittest
{

#define CHECK_EXCEPTION(S, E, M)                                        \
    BOOST_CHECK_EXCEPTION(                                              \
        S,                                                              \
        E,                                                              \
        [](const auto& e) { BOOST_TEST(e.what() == M); return true; });

// TODO: also test with h4, h2 and h1
static const std::vector<unsigned char> h8_encoded_data
{
    0x28, 0x1b, 0x00, 0x00, 0x0d, 0x00, 0x00, 0xc1, 0x41, 0x02, 0x20, 0x6f,
    0x41, 0x73, 0x01, 0xc2, 0x42, 0x65, 0xc2, 0xc3, 0x66, 0x74, 0xc2, 0x2e,
    0x6c, 0x77, 0x62, 0x68, 0x48, 0x61, 0x00, 0x00, 0x9b, 0xaf, 0x64, 0x00,
    0xcd, 0x4b, 0x80, 0xcc, 0x08, 0x97, 0x50, 0xe6
};

static const std::vector<unsigned char> unencoded_data
{
    'H', 'e', ' ', 'w', 'h', 'o', ' ', 'f', 'o', 'o', 's', ' ', 'l', 'a',
    's', 't', ' ', 'f', 'o', 'o', 's', ' ', 'b', 'e', 's', 't', '.',
};

class huffman_decoder_test_fixture
{
public:
    shrinklergba::huffman_decoder decoder;
};

BOOST_FIXTURE_TEST_SUITE(huffman_decoder_test, huffman_decoder_test_fixture)

    BOOST_AUTO_TEST_CASE(decode_when_compression_type_is_wrong_then_throws)
    {
        std::vector<unsigned char> data{ 0x38, 0x00, 0x00, 0x00 };
        CHECK_EXCEPTION(decoder.decode(data), std::runtime_error, "invalid compression type");
    }

    BOOST_AUTO_TEST_CASE(decode_when_symbol_size_is_wrong_then_throws)
    {
        std::vector<unsigned char> data{ 0x27, 0x00, 0x00, 0x00 };
        CHECK_EXCEPTION(decoder.decode(data), std::runtime_error, "invalid symbol size");
    }

    BOOST_AUTO_TEST_CASE(decode_c_h8)
    {
        BOOST_TEST(unencoded_data == decoder.decode_c(h8_encoded_data), boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(decode_h8)
    {
        BOOST_TEST(unencoded_data == decoder.decode(h8_encoded_data), boost::test_tools::per_element());
    }

BOOST_AUTO_TEST_SUITE_END()

}

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
#include <stdexcept>
#include <vector>
#include "shrinklergba/huffman.hpp"
#include "test_utilities.hpp"

namespace shrinklergba_unittest
{

static const std::vector<unsigned char> h1_encoded_data
{
    0x21, 0x1a, 0x00, 0x00, 0x01, 0xc0, 0x00, 0x01, 0x04, 0x34, 0x16, 0xf2,
    0xe4, 0xae, 0xf6, 0x9e, 0x76, 0x04, 0xa6, 0x4e, 0xae, 0x66, 0x04, 0xf6,
    0x76, 0x86, 0x04, 0x76, 0x4e, 0xf6, 0xb6, 0x9e, 0x00, 0x00, 0x74, 0xa6
};

static const std::vector<unsigned char> h2_encoded_data
{
    0x22, 0x0c, 0x00, 0x00, 0x03, 0x00, 0xc0, 0xc1, 0x01, 0x03, 0x02, 0x00,
    0xfb, 0xdb, 0xe8, 0x5c, 0xe8, 0xfb, 0x98, 0x38, 0x9b, 0xf4, 0x58, 0x58
};

static const std::vector<unsigned char> h4_encoded_data
{
    0x24, 0x1a, 0x00, 0x00, 0x0d, 0x00, 0x00, 0x81, 0x41, 0x82, 0x06, 0xc2,
    0x02, 0x07, 0x02, 0xc2, 0x00, 0x05, 0x41, 0xc2, 0x03, 0x04, 0x41, 0x09,
    0x0e, 0x0f, 0xc0, 0x01, 0x08, 0x0d, 0x00, 0x00, 0x30, 0x48, 0xe4, 0x0b,
    0x1d, 0x39, 0x39, 0xa7, 0xc9, 0x09, 0xb9, 0x92, 0xb2, 0x37, 0xe0, 0x88,
    0x31, 0xef, 0x05, 0x68, 0x00, 0x00, 0x90, 0xf0
};

static const std::vector<unsigned char> h8_encoded_data
{
    0x28, 0x1b, 0x00, 0x00, 0x0d, 0x00, 0x00, 0xc1, 0x41, 0x02, 0x20, 0x6f,
    0x41, 0x73, 0x01, 0xc2, 0x42, 0x65, 0xc2, 0xc3, 0x66, 0x74, 0xc2, 0x2e,
    0x6c, 0x77, 0x62, 0x68, 0x48, 0x61, 0x00, 0x00, 0x9b, 0xaf, 0x64, 0x00,
    0xcd, 0x4b, 0x80, 0xcc, 0x08, 0x97, 0x50, 0xe6
};

static auto h1_decoded_data = make_bytevector("Oh, you're no fun anymore.");

static auto h2_decoded_data = make_bytevector("Oh, an hoop.");

static auto h4_decoded_data = make_bytevector("I want to buy some cheese.");

static auto h8_decoded_data = make_bytevector("He who foos last foos best.");

class huffman_decoder_test_fixture
{
public:
    shrinklergba::huffman_decoder decoder;
};

BOOST_FIXTURE_TEST_SUITE(huffman_decoder_test, huffman_decoder_test_fixture)

    BOOST_AUTO_TEST_CASE(decode_when_compressed_data_is_too_short_then_throws)
    {
        std::vector<unsigned char> data{ 0x28, 0x00, 0x00 };
        CHECK_EXCEPTION(decoder.decode(data), std::runtime_error, "compressed data is too short");
    }

    BOOST_AUTO_TEST_CASE(decode_when_compressed_data_size_is_not_aligned_then_throws)
    {
        std::vector<unsigned char> data{ 0x28, 0x00, 0x00, 0x00, 0x00 };
        CHECK_EXCEPTION(decoder.decode(data), std::runtime_error, "compressed data size is not a multiple of 4 bytes");
    }

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

    BOOST_AUTO_TEST_CASE(decode_h1)
    {
        BOOST_TEST(h1_decoded_data == decoder.decode(h1_encoded_data), boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(decode_h2)
    {
        BOOST_TEST(h2_decoded_data == decoder.decode(h2_encoded_data), boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(decode_h4)
    {
        BOOST_TEST(h4_decoded_data == decoder.decode(h4_encoded_data), boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(decode_h8)
    {
        BOOST_TEST(h8_decoded_data == decoder.decode(h8_encoded_data), boost::test_tools::per_element());
    }

BOOST_AUTO_TEST_SUITE_END()

}

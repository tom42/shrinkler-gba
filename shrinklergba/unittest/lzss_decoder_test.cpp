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
#include <cstring>
#include <vector>
#include "shrinklergba/lzss.hpp"
#include "test_utilities.hpp"

namespace shrinklergba_unittest
{

// TODO: share with huffman_decoder_test?
static const std::vector<unsigned char> make_bytevector(const char* s)
{
    return std::vector<unsigned char>(s, s + std::strlen(s));
}

static const std::vector<unsigned char> zero_bytes_encoded_data
{
    0x10, 0x00, 0x00, 0x00
};

static const std::vector<unsigned char> one_literal_encoded_data
{
    0x10, 0x01, 0x00, 0x00, 0x00, 0x41, 0x00, 0x00
};

static const std::vector<unsigned char> nine_literals_encoded_data
{
    0x10, 0x09, 0x00, 0x00, 0x00, 0x41, 0x42, 0x43,
    0x44, 0x45, 0x46, 0x47, 0x48, 0x00, 0x49, 0x00
};

static const std::vector<unsigned char> literals_and_references_encoded_data
{
    0x10, 0x37, 0x00, 0x00, 0x00, 0x48, 0x65, 0x20,
    0x77, 0x68, 0x6f, 0x20, 0x66, 0x00, 0x6f, 0x6f,
    0x73, 0x20, 0x6c, 0x61, 0x73, 0x74, 0x81, 0x30,
    0x09, 0x62, 0x65, 0x73, 0x74, 0x2e, 0x20, 0xf0,
    0x1b, 0x80, 0x60, 0x1b,
};

static const std::vector<unsigned char> zero_bytes_decoded_data;

static const std::vector<unsigned char> one_literal_decoded_data =
    make_bytevector("A");

static const std::vector<unsigned char> nine_literals_decoded_data =
    make_bytevector("ABCDEFGHI");

static const std::vector<unsigned char> literals_and_references_decoded_data =
    make_bytevector("He who foos last foos best. He who foos last foos best.");

class lzss_decoder_test_fixture
{
public:
    shrinklergba::lzss_decoder decoder;
};

BOOST_FIXTURE_TEST_SUITE(lzss_decoder_test, lzss_decoder_test_fixture)

// TODO: tests:
//       * reserved bits in header must be 0 (orly?)
//       * Actually decompress something
//         * Larger run of data with literals and references and whatnot

    BOOST_AUTO_TEST_CASE(decode_when_compressed_data_is_too_short_then_throws)
    {
        std::vector<unsigned char> data{ 0x10, 0x00, 0x00 };
        CHECK_EXCEPTION(decoder.decode(data), std::runtime_error, "compressed data is too short");
    }

    BOOST_AUTO_TEST_CASE(decode_when_compressed_data_size_is_not_aligned_then_throws)
    {
        std::vector<unsigned char> data{ 0x10, 0x00, 0x00, 0x00, 0x00 };
        CHECK_EXCEPTION(decoder.decode(data), std::runtime_error, "compressed data size is not a multiple of 4 bytes");
    }

    BOOST_AUTO_TEST_CASE(decode_when_compression_type_is_wrong_then_throws)
    {
        std::vector<unsigned char> data{ 0x00, 0x00, 0x00, 0x00 };
        CHECK_EXCEPTION(decoder.decode(data), std::runtime_error, "invalid compression type");
    }

    BOOST_AUTO_TEST_CASE(decode_zero_bytes)
    {
        BOOST_TEST(zero_bytes_decoded_data == decoder.decode(zero_bytes_encoded_data), boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(decode_one_literal)
    {
        BOOST_TEST(one_literal_decoded_data == decoder.decode(one_literal_encoded_data), boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(decode_nine_literals)
    {
        BOOST_TEST(nine_literals_decoded_data == decoder.decode(nine_literals_encoded_data), boost::test_tools::per_element());
    }

    BOOST_AUTO_TEST_CASE(decode_literals_and_references)
    {
        BOOST_TEST(literals_and_references_decoded_data == decoder.decode(literals_and_references_encoded_data), boost::test_tools::per_element());
    }

BOOST_AUTO_TEST_SUITE_END()

}

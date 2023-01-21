// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include <boost/test/unit_test.hpp>
#include <cstring>
#include <vector>
#include "shrinklergbacore/lzss.hpp"
#include "test_utilities.hpp"

namespace shrinklergbacore_unittest
{

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

static auto zero_bytes_decoded_data = make_bytevector("");

static auto one_literal_decoded_data = make_bytevector("A");

static auto nine_literals_decoded_data = make_bytevector("ABCDEFGHI");

static auto literals_and_references_decoded_data = make_bytevector("He who foos last foos best. He who foos last foos best.");

class lzss_decoder_test_fixture
{
public:
    shrinklergbacore::lzss_decoder decoder;
};

BOOST_FIXTURE_TEST_SUITE(lzss_decoder_test, lzss_decoder_test_fixture)

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

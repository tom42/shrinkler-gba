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

#include <array>
#include <boost/endian/conversion.hpp>
#include <stdexcept>
#include "shrinklergba/huffman.hpp"

namespace shrinklergba
{

enum class compression_type : unsigned char
{
    huffman = 2
};

// Offsets relative to start of header
constexpr std::size_t ofs_compression_type = 0;
constexpr std::size_t ofs_decompressed_size = 1;
constexpr std::size_t ofs_tree_size = 4;

// Bit masks
constexpr unsigned char mask_left = 0x80;
constexpr unsigned char mask_right = 0x40;
constexpr unsigned char mask_next_node_offset = 63;

std::vector<unsigned char> huffman_decoder::decode(const std::vector<unsigned char>& compressed_data)
{
    return decode(compressed_data.data(), compressed_data.size());
}

std::vector<unsigned char> huffman_decoder::decode(const unsigned char* compressed_data, std::size_t size)
{
    check_compressed_size(size);
    check_compression_type(compressed_data[ofs_compression_type] >> 4);
    const int symbol_size = check_symbol_size(compressed_data[ofs_compression_type] & 15);
    std::size_t decompressed_size = get_decompressed_size(compressed_data);

    tree_size = compressed_data + ofs_tree_size;
    // TODO: check on SO: is this legal?
    readptr = reinterpret_cast<const uint32_t*>(compressed_data + ofs_tree_size + 2 * (compressed_data[ofs_tree_size] + 1));
    std::vector<unsigned char> decompressed_data;
    decompressed_data.reserve(decompressed_size);
    bitbuffer = 0;
    bitmask = 0;

    for (size_t i = 0; i < decompressed_size; ++i)
    {
        unsigned char decoded_byte = 0;
        for (int nbits = 0; nbits < 8; nbits += symbol_size)
        {
            decoded_byte |= decode_symbol() << nbits;
        }
        decompressed_data.push_back(decoded_byte);
    }

    return decompressed_data;
}

unsigned char huffman_decoder::decode_symbol()
{
    std::size_t current_node_index = 0;
    unsigned char current_node_value = tree_size[1];
    bool character_found = false;

    while (!character_found)
    {
        current_node_index += 2 * ((current_node_value & mask_next_node_offset) + 1);

        if (!get_bit())
        {
            character_found = current_node_value & mask_left;
            current_node_value = tree_size[current_node_index];
        }
        else
        {
            character_found = current_node_value & mask_right;
            current_node_value = tree_size[current_node_index + 1];
        }
    }

    // TODO: Data (upper bits should be zero if Data Size is less than 8) => So we have two possibilities here:
    //       * Safe mode: we bark, because it is an error
    //       * Robust mode: we mask out high bits that should be 0
    return current_node_value;
}

bool huffman_decoder::get_bit()
{
    bitmask >>= 1;
    if (!bitmask)
    {
        bitmask = 0x80000000;   // TODO: constant?
        // TODO: should check whether we do not read past end of compressed data before performing this read! (if we do, throw)
        bitbuffer = boost::endian::little_to_native(*readptr++);
    }

    return bitbuffer & bitmask;
}

void huffman_decoder::check_compressed_size(std::size_t compressed_size)
{
    // Check the bare minimum size required for the compression header.
    // In reality it's more complicated. If the decompressed size is 0, then 4 bytes is enough.
    // If the decompressed size is nonzero, then the minimum size is more than 4 bytes.
    if (compressed_size < 4)
    {
        throw std::runtime_error("invalid compressed data");
    }
    // TODO: more size checks? For instance, the size should be a multiple of 4 since encoded data is 32 bit aligned, no?
    //       * Yes but then should probably also check the pointer to the compressed data, no?
}

void huffman_decoder::check_compression_type(unsigned char type)
{
    if (static_cast<compression_type>(type) != compression_type::huffman)
    {
        throw std::runtime_error("invalid compression type");
    }
}

int huffman_decoder::check_symbol_size(int symbol_size)
{
    static const std::array valid_sizes{ 1, 2, 4, 8 };

    if (std::find(valid_sizes.begin(), valid_sizes.end(), symbol_size) == valid_sizes.end())
    {
        throw std::runtime_error("invalid symbol size");
    }

    return symbol_size;
}

std::size_t huffman_decoder::get_decompressed_size(const unsigned char* compressed_data)
{
    unsigned char b0 = compressed_data[ofs_decompressed_size + 0];
    unsigned char b1 = compressed_data[ofs_decompressed_size + 1];
    unsigned char b2 = compressed_data[ofs_decompressed_size + 2];

    auto size = b0 + (b1 << 8) + (b2 << 16);
    return size;
}

}

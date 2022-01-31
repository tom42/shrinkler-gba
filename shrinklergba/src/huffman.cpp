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

#include <algorithm>
#include <array>
#include <iostream> // TODO: remove
#include "shrinklergba/huffman.hpp"

namespace shrinklergba
{

std::vector<unsigned char> huffman_decoder::decode(const std::vector<unsigned char>& data) const
{
    auto input = data.begin();

    check_compression_type(*input >> 4);

    const int symbol_size = *input++ & 15;
    check_symbol_size(symbol_size);

    const size_t decompressed_size = get_decompressed_size(input);
    const size_t tree_size = get_tree_size(input);

    const auto tree_root = input;

    // TODO: skip to beginning of compressed data

    // TODO: decode data
    //       * We know the input data size, so we can have a loop for this
    //         * Yes that's OK, but remember, we have total size in bytes, but symbols which may be smaller than 8 bits!
    //       * Get symbol (we need to be able to read the input bit wise)
    //       * Write symbol (we need to be able to write the output bit wise)

    // TODO: remove all logging
    std::cout << "Decompressed size: " << decompressed_size << " bytes" << std::endl;
    std::cout << "Tree size:         " << tree_size << " bytes" << std::endl;

    return std::vector<unsigned char>();
}

void huffman_decoder::check_compression_type(unsigned char compression_type) const
{
    if (compression_type != 2)
    {
        throw std::runtime_error("invalid compression type");
    }
}

void huffman_decoder::check_symbol_size(int symbol_size) const
{
    static const std::array valid_sizes{ 1, 2, 4, 8 };

    if (std::find(valid_sizes.begin(), valid_sizes.end(), symbol_size) == valid_sizes.end())
    {
        throw std::runtime_error("invalid symbol size");
    }
}

size_t huffman_decoder::get_decompressed_size(std::vector<unsigned char>::const_iterator& i) const
{
    unsigned char b0 = *i++;
    unsigned char b1 = *i++;
    unsigned char b2 = *i++;

    // TODO: this ought to be tested also for bits 8..23!
    auto size = b0 + (b1 << 8) + (b2 << 16);
    return size;
}

size_t huffman_decoder::get_tree_size(std::vector<unsigned char>::const_iterator& i) const
{
    auto size = *i++;
    // TODO: this is NOT the real size. We must calculate that. Problem: is it
    //       * (size + 1) * 2
    //       * Or (size * 2) + 1
    return size;
}

}

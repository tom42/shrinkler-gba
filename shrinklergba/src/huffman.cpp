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

#include <iostream> // TODO: remove
#include "shrinklergba/huffman.hpp"

namespace shrinklergba
{

std::vector<unsigned char> huffman_decoder::decode(const std::vector<unsigned char>& data)
{
    auto input = data.begin();

    // TODO: read first part of header: compression type
    // TODO: throw if wrong compression type
    auto compression_type = *input >> 4;
    std::cout << compression_type << std::endl; // TODO: remove

    // TODO: read symbol width
    // TODO: throw if wrong symbol width (that is, neither of 1, 2, 4, 8? Probably? For starters we could probably also go for just 8, and then later 4)
    auto symbol_width = *input++ & 15;
    std::cout << symbol_width << std::endl; // TODO: remove

    // TODO: read size of decompressed data
    // TODO: this is broken (UB, for one thing) => Own method, and maybe unit test it?
    // TODO: also it is maybe time to stop using auto here. Anyway it is not needed to use auto all over the place here
    auto decompressed_size = *input++ + *input++ * 256 + *input++ * 65536;
    std::cout << decompressed_size << std::endl; // TODO: Remove

    // TODO: decode data
    //       * We know the input data size, so we can have a loop for this
    //       * Get symbol (we need to be able to read the input bit wise)
    //       * Write symbol (we need to be able to write the output bit wise)

    return std::vector<unsigned char>();
}

}

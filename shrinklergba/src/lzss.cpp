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

#include <stdexcept>
#include "shrinklergba/gba_bios_common.hpp"
#include "shrinklergba/lzss.hpp"

namespace shrinklergba
{

std::vector<unsigned char> lzss_decoder::decode(const std::vector<unsigned char>& compressed_data)
{
    return decode(compressed_data.data(), compressed_data.size());
}

std::vector<unsigned char> lzss_decoder::decode(const unsigned char* compressed_data, std::size_t size)
{
    throw_if_wrong_compressed_size(size);
    throw_if_wrong_compression_type(compression_type::lzss, compressed_data[ofs_compression_type] >> 4);
    const std::size_t decompressed_size = get_decompressed_size(compressed_data);
    
    std::vector<unsigned char> decompressed_data;
    decompressed_data.reserve(decompressed_size);

    // TODO: real decoding loop must look different, due to references.
    //       if these are bad we can jump beyond the decompressed size, so we must catch this too

    unsigned char tagbits = 0;
    unsigned char bitmask = 0;
    readptr = compressed_data + 4; // TODO: constant for magic number 4
    readptr_end = compressed_data + size;

    while (decompressed_data.size() < decompressed_size)
    {
        bitmask >>= 1;
        if (!bitmask)
        {
            tagbits = read_byte();
            bitmask = 0x80;
        }

        if (tagbits & bitmask)
        {
            unsigned char b0 = read_byte();
            unsigned char b1 = read_byte();
            std::size_t length = ((b0 >> 4) & 15) + 3;
            std::size_t offset = ((b0 & 15) << 8) | b1;

            if (decompressed_data.size() + length > decompressed_size)
            {
                throw std::runtime_error("wrong match length");
            }

            while (length--)
            {
                // TODO: do we need to catch write past decompressed_size? (do not think so, though => can do finally, no?)
                decompressed_data.push_back(decompressed_data[decompressed_data.size() - offset - 1]);
            }
        }
        else
        {
            // TODO: do we need to catch write past decompressed_size? (do not think so, though => can do finally, no?)
            decompressed_data.push_back(read_byte());
        }
    }

    // TODO: sanity check: actually decompressed size = desired decompressed size?

    return decompressed_data;
}

unsigned char lzss_decoder::read_byte()
{
    if (readptr >= readptr_end)
    {
        throw std::runtime_error("unexpected end of compressed data");
    }

    return *readptr++;
}

}

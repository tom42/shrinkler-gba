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
#include "shrinklergba/huffman.hpp"

namespace shrinklergba
{

std::vector<unsigned char> huffman_decoder::decode_c(const std::vector<unsigned char>& data) const
{
    constexpr auto CMD_CODE_28 = 0x28;      // 8-bits Huffman magic number
    constexpr auto CMD_CODE_24 = 0x24;      // 4-bits Huffman magic number
    constexpr auto CMD_CODE_22 = 0x22;      // 2-bits Huffman magic number (test mode)
    constexpr auto CMD_CODE_21 = 0x21;      // 1-bit  Huffman magic number (test mode)
    //constexpr auto HUF_MINIM = 0x00000004;  // empty RAW file (header only)
    //constexpr auto HUF_MAXIM = 0x01400000;  // 0x01000203, padded to 20MB:
    constexpr auto HUF_SHIFT = 1;           // bits to shift
    constexpr auto HUF_MASK4 = 0x80000000;  // first bit to check (HUF_RNODE << 31)
    constexpr auto HUF_LCHAR = 0x80;        // next lnode is a char, bit 7, (1 << 7)
    constexpr auto HUF_RCHAR = 0x40;        // next rnode is a char, bit 6, (1 << 6)
    constexpr auto HUF_NEXT = 0x3F;         // inc to next node/char (nwords+1), bits 5-0
                                            // * (0xFF & ~(HUF_LCHAR | HUF_RCHAR))

    const unsigned char* pak_buffer, * pak, * pak_end;
    unsigned char  *raw_buffer, *raw, *raw_end;
    unsigned int   header;
    size_t pak_len, raw_len;
    const unsigned char* tree;
    unsigned int   pos, next, mask4, code, ch, nbits;
    unsigned int    num_bits;

    pak_buffer = data.data();
    pak_len = data.size();

    header = *pak_buffer;
    if ((header != CMD_CODE_22) && (header != CMD_CODE_21))
        if ((header != CMD_CODE_24) && (header != CMD_CODE_28)) {
            free((void*)pak_buffer);
            throw std::runtime_error("WARNING: file is not Huffman encoded!");
        }

    num_bits = header & 0xF;

    raw_len = *(unsigned int*)pak_buffer >> 8;
    raw_buffer = (unsigned char*)malloc(raw_len * sizeof(char));
    memset(raw_buffer, 0, raw_len * sizeof(char));

    pak = pak_buffer + 4;
    raw = raw_buffer;
    pak_end = pak_buffer + pak_len;
    raw_end = raw_buffer + raw_len;

    tree = pak; // Note: tree does intentionally not point to the tree root, but one byte in front of it.
    pak += (size_t)(*pak + 1) << 1;

    nbits = 0;

    pos = *(tree + 1);
    next = 0;
    code = 0; // Unnecessary, but shut up about potentially uninitialized variable

    mask4 = 0;
    while (raw < raw_end) {
        if (!(mask4 >>= HUF_SHIFT)) {
            if (pak + 3 >= pak_end) break;
            code = *(unsigned int*)pak;
            pak += 4;
            mask4 = HUF_MASK4;
        }

        next += ((pos & HUF_NEXT) + 1) << 1;

        if (!(code & mask4)) {
            ch = pos & HUF_LCHAR;
            pos = *(tree + next);
        }
        else {
            ch = pos & HUF_RCHAR;
            pos = *(tree + next + 1);
        }

        if (ch) {
            *raw |= pos << nbits;
            ////  *raw = (*raw << num_bits) | pos; 
            nbits = (nbits + num_bits) & 7;
            if (!(nbits))
            {
                raw++;
            }

            pos = *(tree + 1);
            next = 0;
        }
    }

    raw_len = raw - raw_buffer;

    if (raw != raw_end) printf(", WARNING: unexpected end of encoded file!");

    std::vector<unsigned char> result(raw_buffer, raw_buffer + raw_len);


    free(raw_buffer);

    return result;
}

std::vector<unsigned char> huffman_decoder::decode(const std::vector<unsigned char>& /*data*/) const
{
    throw std::runtime_error("YIKES");
/*
    // TODO: this is not getting us anywhere
    //       * Stop using iterators. We will not use this on non-contiguous memory anyway, so we can just as well use pointers, or array indexes. Much simpler to deal with.
    //       * Get a reference implementation going, so we can more easily reverse engineer crap. No need to decode gbatek once more by ourselves.

    auto input = data.begin();

    check_compression_type(*input >> 4);

    const int symbol_size = *input++ & 15;
    check_symbol_size(symbol_size);

    const size_t decompressed_size = get_decompressed_size(input);
    const size_t tree_size = get_tree_size(input);
    input += tree_size - 1; // TODO: it is a mess what we have atm, where iterators are incremented. Should we maybe fix this?

    const auto tree_root = input;

    // TODO: decode data
    //       * We know the input data size, so we can have a loop for this
    //         * Yes that's OK, but remember, we have total size in bytes, but symbols which may be smaller than 8 bits!
    //       * Get symbol (we need to be able to read the input bit wise)
    //       * Write symbol (we need to be able to write the output bit wise)

    // TODO: remove all logging
    std::cout << "Decompressed size: " << decompressed_size << " bytes" << std::endl;
    std::cout << "Tree size:         " << tree_size << " bytes" << std::endl;
    std::cout << "First byte:        " << (int)(*input) << std::endl;

    return std::vector<unsigned char>();
*/
}

// TODO: remove
/*
void huffman_decoder::check_compression_type(unsigned char compression_type) const
{
    if (compression_type != 2)
    {
        throw std::runtime_error("invalid compression type");
    }
}
*/

// TODO: remove
/*
void huffman_decoder::check_symbol_size(int symbol_size) const
{
    static const std::array valid_sizes{ 1, 2, 4, 8 };

    if (std::find(valid_sizes.begin(), valid_sizes.end(), symbol_size) == valid_sizes.end())
    {
        throw std::runtime_error("invalid symbol size");
    }
}
*/

// TODO: remove
/*
size_t huffman_decoder::get_decompressed_size(std::vector<unsigned char>::const_iterator& i) const
{
    unsigned char b0 = *i++;
    unsigned char b1 = *i++;
    unsigned char b2 = *i++;

    // TODO: this ought to be tested also for bits 8..23!
    auto size = b0 + (b1 << 8) + (b2 << 16);
    return size;
}
*/

// TODO: remove
// TODO: not sure this is the right name: not sure this is a size, or actually an offset
//       * I think it is the latter, an offset, because data is processed 32 bit wise
//       * Well if this turns out to be the case, rename this method
/*
size_t huffman_decoder::get_tree_size(std::vector<unsigned char>::const_iterator& i) const
{
    auto size = (*i + 1) * 2;
    ++i;
    return size;
}
*/

}

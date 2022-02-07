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
#include <iostream> // TODO: remove
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
constexpr std::size_t ofs_tree_root = 5;

// TODO: what is this?
constexpr unsigned char mask_left = 0x80;
constexpr unsigned char mask_right = 0x40;

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
    std::size_t pak_len, raw_len;
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
    pak += (std::size_t)(*pak + 1) << 1;

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

        unsigned int found_pos;
        if (!(code & mask4)) {
            //std::cout << "moving left, pos is " << pos << std::endl;
            ch = pos & HUF_LCHAR;
            pos = *(tree + next);
            found_pos = next;
        }
        else {
            //std::cout << "moving right, pos is " << pos << std::endl;
            ch = pos & HUF_RCHAR;
            pos = *(tree + next + 1);
            found_pos = next + 1;
        }

        if (ch) {
            //std::cout << "found <" << (char)pos << "> at " << found_pos << std::endl;
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

std::vector<unsigned char> huffman_decoder::decode(const std::vector<unsigned char>& compressed_data)
{
    return decode(compressed_data.data(), compressed_data.size());
}

std::vector<unsigned char> huffman_decoder::decode(const unsigned char* compressed_data, std::size_t /*size*/)
{
    // TODO: assert address is a multiple of 4?
    // TODO: in principle should check a minimum size here, since we're going to access the header right away
    check_compression_type(compressed_data[ofs_compression_type] >> 4);
    check_symbol_size(compressed_data[ofs_compression_type] & 15);
    std::size_t decompressed_size = get_decompressed_size(compressed_data);

    tree_root = compressed_data + ofs_tree_root;
    // TODO: is this legal?
    // TODO: maybe at least have an assertion that readptr is aligned? (then again, the CPU will complain I guess)
    readptr = reinterpret_cast<const uint32_t*>(compressed_data + ofs_tree_size + 2 * (compressed_data[ofs_tree_size] + 1));
    std::vector<unsigned char> decompressed_data;
    decompressed_data.reserve(decompressed_size);
    bitbuffer = 0;
    bitmask = 0;

    // TODO: real decompression loop: honor decompressed data size
    for (int i = 0; i < 27; ++i)
    {
        // TODO: instead of printing the character, stick it into the output buffer
        // TODO: write symbol to output. Later we must take into account that symbol size may be < 8 bits
        decompressed_data.push_back(decode_symbol());
    }

    return decompressed_data;
}

void huffman_decoder::check_compression_type(unsigned char type) const
{
    if (static_cast<compression_type>(type) != compression_type::huffman)
    {
        throw std::runtime_error("invalid compression type");
    }
}

int huffman_decoder::check_symbol_size(int symbol_size) const
{
    static const std::array valid_sizes{ 1, 2, 4, 8 };

    if (std::find(valid_sizes.begin(), valid_sizes.end(), symbol_size) == valid_sizes.end())
    {
        throw std::runtime_error("invalid symbol size");
    }

    return symbol_size;
}

std::size_t huffman_decoder::get_decompressed_size(const unsigned char* compressed_data) const
{
    unsigned char b0 = compressed_data[ofs_decompressed_size + 0];
    unsigned char b1 = compressed_data[ofs_decompressed_size + 1];
    unsigned char b2 = compressed_data[ofs_decompressed_size + 2];

    // TODO: at the very least manually check this for bits 8..23, no?
    auto size = b0 + (b1 << 8) + (b2 << 16);
    return size;
}

unsigned char huffman_decoder::decode_symbol()
{
    const unsigned char* current_node = tree_root;

    bool character_found = false;
    while (!character_found)
    {
        bitmask >>= 1;
        if (!bitmask)
        {
            bitmask = 0x80000000;   // TODO: constant?
            // TODO: should check whether we do not read past end of compressed data before performing this read! (if we do, throw)
            bitbuffer = *readptr++; // TODO: little/big endian: source data is little endian; if we're on a big endian machine we must perform little to big endian conversion here.
        }

        if (!(bitbuffer & bitmask))
        {
            // TODO: OK, this seems to work, it's just ugly as sin
            character_found = *current_node & mask_left;
            auto ofs = *current_node & 63;              // TODO: constant: 63. Also, line is duplicated
            // TODO: ugly pointer castery, should be possible without this
            uintptr_t foo = (uintptr_t)current_node;
            foo = (foo & ~1) + ofs * 2 + 2;
            current_node = (unsigned char*)foo;
        }
        else
        {
            character_found = *current_node & mask_right;
            auto ofs = *current_node & 63;              // TODO: constant: 63. Also, line is duplicated
            // TODO: ugly pointer castery, should be possible without this
            uintptr_t foo = (uintptr_t)current_node;
            foo = (foo & ~1) + ofs * 2 + 2 + 1;         // TODO: watch out: +1, compared to branch above
            current_node = (unsigned char*)foo;
        }
    }

    // TODO: Data (upper bits should be zero if Data Size is less than 8) => So we have two possibilities here:
    //       * Safe mode: we bark, because it is an error
    //       * Robust mode: we mask out high bits that should be 0
    return *current_node;
}

}

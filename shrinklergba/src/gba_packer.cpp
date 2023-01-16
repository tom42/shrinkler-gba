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

#include <cassert>
#include <cerrno>
#include <fstream>
#include <iostream>
#include <string>
#include <system_error>
#include <vector>
#include "fmt/core.h"
#include "lzasm/arm/arm32/divided_thumb_assembler.hpp"
#include "shrinkler/shrinkler.hpp"
#include "shrinklergba/console.hpp"
#include "shrinklergba/gba_packer.hpp"
#include "shrinklergba/input_file.hpp"

namespace shrinklergba
{

// Offsets in GBA cartridge header
constexpr size_t ofs_game_title = 0xa0;
constexpr size_t ofs_fixed_byte = 0xb2;
constexpr size_t ofs_device_type = 0xb4;
constexpr size_t ofs_game_version = 0xbc;
constexpr size_t ofs_complement = 0xbd;
constexpr size_t ofs_reserved2 = 0xbe;

void gba_packer::pack(const options& options)
{
    // TODO: Try Shrinkler and LZSS+H4/H8 compression, select which is better
    console console;
    console.verbose(options.verbose() ? &std::cout : nullptr);

    input_file input_file(console);
    input_file.load(options.input_file());

    std::vector<unsigned char> cart = make_shrinklered_cart(input_file, options);
    write_complement(cart);

    CONSOLE_VERBOSE(console) << "Writing: " << options.output_file().string() << std::endl;
    write_to_disk(cart, options.output_file());
}

void gba_packer::write_complement(std::vector<unsigned char>& cart)
{
    char complement = 0;
    for (size_t n = ofs_game_title; n < ofs_complement; ++n)
    {
        complement += cart[n];
    }

    cart[ofs_complement] = -(0x19 + complement);
}

void gba_packer::write_to_disk(const std::vector<unsigned char>& data, const std::filesystem::path& filename)
{
    try
    {
        std::ofstream file;
        file.open(filename.string(), std::ios::binary | std::ios::trunc);
        if (!file)
        {
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        file.write(reinterpret_cast<const char*>(&data[0]), data.size());
        if (!file)
        {
            auto e = errno;
            throw std::system_error(e, std::generic_category());
        }

        file.close();
    }
    catch (const std::system_error& e)
    {
        remove_output_file(filename);
        throw std::runtime_error(fmt::format("could not write {}: {}", filename.string(), e.what()));
    }
}

void gba_packer::remove_output_file(const std::filesystem::path& filename)
{
    // Delete file, ignore any error. Use overload of remove that does not throw.
    std::error_code e;
    std::filesystem::remove(filename, e);
}

std::vector<unsigned char> gba_packer::make_shrinklered_cart(const input_file& input_file, const options& options)
{
    using namespace lzasm::arm::arm32;
    using namespace std::literals::string_literals;

    // TODO: implement:
    //       * Create a shrinklered intro, taking into account
    //         * RESTORE STACK POINTER (maybe check all registers)
    //           * As an idea we can have this as an option, since it saves at least one instruction
    //         * Possibly optional features
    //           * Preserve registers? Low registers are easy (sure?, what about high registers?
    //             * Well probably register contents as left by the BIOS are known, so we could just mov/ldr original values (I think there is no smaller instruction)
    //           * Preserve memory contents?
    //         * Stick code into header
    //         * Possible optimizations:
    //           * If load address and entry point are the same
    //           * If load address and/or entry point can be constructed using mov/lsl
    //         * Note: getnumber/getkind/getbit are less likely to be changed by binary dependend optimizations or user options such as whether registers should be saved
    //           * So if we turn around the decoder and stick e.g. getnumber into the header, the main decoding loop will be outside the header, making it more easy to apply optimizations.
    //           * By doing so we will not lose anything size-wise, since at the cartridge beginning we need to have a branch anyway
    //         * Debug mode: selectable through command line
    //           * E.g. check stack pointer before branching to depacked intro
    shrinkler::shrinkler_compressor compressor;
    compressor.set_parameters(options.shrinkler_parameters());
    std::vector<unsigned char> compressed_program = compressor.compress(input_file.data());

    divided_thumb_assembler a;

    ////////////////////////////////////////////////////////////////////////////
    // Constants
    ////////////////////////////////////////////////////////////////////////////

    // 1536 contexts would be sufficient, but 2048 is smaller.
    //constexpr auto INIT_ONE_PROB = 0x8000;
    constexpr auto ADJUST_SHIFT = 4;
    constexpr auto SINGLE_BIT_CONTEXTS = 1;
    //constexpr auto NUM_CONTEXTS = 2048;

    // Register aliases
    constexpr auto inp = r0;                // Compressed data
    constexpr auto outp = r1;               // Decompressed data
    constexpr auto tmp0 = r2;               // Scratch register 0
    constexpr auto tmp1 = r3;               // Scratch register 1
    constexpr auto rvalue = r4;             // Range value
    constexpr auto isize = r5;              // Interval size
    constexpr auto bitbuf = r6;             // Input bit buffer
    constexpr auto bitctx = r7;             // Bit context index
    constexpr auto offset = r8;             // Offset

    constexpr auto getnumber_push_list = make_push_list(outp, lr);
    constexpr auto getbit_push_list = make_push_list(outp, tmp0, tmp1, lr);

    // Offset for accessing the context table relative to SP.
    constexpr auto CTX_TABLE_OFFSET = 4 * getbit_push_list.size() + 4 * getnumber_push_list.size() + 2 * SINGLE_BIT_CONTEXTS;

    ////////////////////////////////////////////////////////////////////////////
    // Cartridge header, with beginning of decompression code
    ////////////////////////////////////////////////////////////////////////////

    a.arm_branch("code_start"s);
    // Logo
    a.byte(0x24, 0xff, 0xae, 0x51, 0x69, 0x9a, 0xa2, 0x21);
    a.byte(0x3d, 0x84, 0x82, 0x0a, 0x84, 0xe4, 0x09, 0xad);
    a.byte(0x11, 0x24, 0x8b, 0x98, 0xc0, 0x81, 0x7f, 0x21);
    a.byte(0xa3, 0x52, 0xbe, 0x19, 0x93, 0x09, 0xce, 0x20);
    a.byte(0x10, 0x46, 0x4a, 0x4a, 0xf8, 0x27, 0x31, 0xec);
    a.byte(0x58, 0xc7, 0xe8, 0x33, 0x82, 0xe3, 0xce, 0xbf);
    a.byte(0x85, 0xf4, 0xdf, 0x94, 0xce, 0x4b, 0x09, 0xc1);
    a.byte(0x94, 0x56, 0x8a, 0xc0, 0x13, 0x72, 0xa7, 0xfc);
    a.byte(0x9f, 0x84, 0x4d, 0x73, 0xa3, 0xca, 0x9a, 0x61);
    a.byte(0x58, 0x97, 0xa3, 0x27, 0xfc, 0x03, 0x98, 0x76);
    a.byte(0x23, 0x1d, 0xc7, 0x61, 0x03, 0x04, 0xae, 0x56);
    a.byte(0xbf, 0x38, 0x84, 0x00, 0x40, 0xa7, 0x0e, 0xfd);
    a.byte(0xff, 0x52, 0xfe, 0x03, 0x6f, 0x95, 0x30, 0xf1);
    a.byte(0x97, 0xfb, 0xc0, 0x85, 0x60, 0xd6, 0x80, 0x25);
    a.byte(0xa9, 0x63, 0xbe, 0x03, 0x01, 0x4e, 0x38, 0xe2);
    a.byte(0xf9, 0xa2, 0x34, 0xff, 0xbb, 0x3e, 0x03, 0x44);
    a.byte(0x78, 0x00, 0x90, 0xcb, 0x88, 0x11, 0x3a, 0x94);
    a.byte(0x65, 0xc0, 0x7c, 0x63, 0x87, 0xf0, 0x3c, 0xaf);
    a.byte(0xd6, 0x25, 0xe4, 0x8b, 0x38, 0x0a, 0xac, 0x72);
    a.byte(0x21, 0xd4, 0xf8, 0x07);

    // Game title (12 bytes), game code (4 bytes) and maker code (2 bytes).
    // In total 18 bytes that can be freely used, so we stick code into them.
    a.label("code_start"s);
    assert(a.current_lc() == ofs_game_title);
    a.arm_to_thumb(inp);            // Switch to Thumb state
    a.mov(isize, 1);                // Initialize range decoder state. rvalue will be set to 0 by the loop that follows.
    a.lsl(bitbuf, isize, 31);       // Bit buffer is empty. Only the sentinel bit is set.

    // Initialize probabilities, writing two contexts per iteration.
    a.lsl(rvalue, isize, 10);       // rvalue = NUM_CONTEXTS / 2
    a.lsl(bitctx, isize, 15);       // bitctx = (INIT_ONE_PROB << 16) | INIT_ONE_PROB
    a.orr(bitctx, bitbuf);          // That is, 0x80008000, so that we can write two contexts at once

    // Fixed byte of value 0x96, followed by unit code which can be freely chosen.
    // We insert an instruction here that contains the required value and that
    // does not bother us otherwise, so that we can execute it rather than jump over it.
    assert(a.current_lc() == ofs_fixed_byte);
    a.mov(tmp0, 0x96);

    // Device type (1 byte), followed by 7 unused bytes.
    assert(a.current_lc() == ofs_device_type);
    a.label("init"s);
    a.push(bitctx);
    a.sub(rvalue, 1);
    a.bne("init"s);
    // Now rvalue is 0
    a.b("reserved2"s);              // Jump over game version and complement

    // Game version (1 byte), followed by the complement/checksum (1 byte).
    // If the complement was known ahead we could attempt to place a harmless
    // Thumb instruction here, but the only instructions we've currently left are
    // those to load the input and output pointers, and those are not constant.
    // Another problem is that the complement goes into the high byte of the instruction
    // and thus dictates the kind of instruction generated, otherwise we could just
    // set up things for this to be a harmless mov instruction.
    assert(a.current_lc() == ofs_game_version);
    a.byte(0x69, 0x00);             // Game version can be freely chosen

    // Reserved
    assert(a.current_lc() == ofs_reserved2);
    a.label("reserved2"s);
    a.adr(inp, "packed_intro"s);    // Initialize input pointer

    ////////////////////////////////////////////////////////////////////////////
    // Remaining decompression code, outside of header
    ////////////////////////////////////////////////////////////////////////////

    // Initialize output pointer
    a.ldr(outp, input_file.load_address());

    // Main decompression loop.
    a.label("literal"s);
    a.mov(tmp0, 1);
    a.label("getlit"s);
    a.lsl(bitctx, outp, 31);        // bitctx = ((outp & 1) << 8) | current_byte
    a.lsr(bitctx, bitctx, 23);
    a.orr(bitctx, tmp0);
    a.bl("getbit"s);
    a.adc(tmp0, tmp0);
    a.cmp(tmp0, 255);
    a.bls("getlit"s);
    a.strb(tmp0, outp, 0);
    a.add(outp, 1);
    a.label("switch"s);
    // After literal
    a.bl("getkind"s);
    a.bcc("literal"s);
    // Reference
    a.mov(bitctx, 0);               // bitctx = -1
    a.sub(bitctx, 1);
    a.bl("getbit"s);
    a.bcc("readoffset"s);
    a.label("readlength"s);
    a.mov(tmp0, 4);
    a.bl("getnumber"s);
    a.mov(tmp0, offset);
    a.neg(tmp0, tmp0);
    a.label("copyloop"s);
    a.ldrb(bitctx, outp, tmp0);
    a.strb(bitctx, outp, 0);
    a.add(outp, 1);
    a.sub(tmp1, 1);
    a.bne("copyloop"s);
    // After reference
    a.bl("getkind"s);
    a.bcc("literal"s);
    a.label("readoffset"s);
    a.mov(tmp0, 3);
    a.bl("getnumber"s);
    a.sub(tmp1, 2);
    a.mov(offset, tmp1);
    a.bne("readlength"s);
    a.label("donedecompressing"s);
    a.ldr(outp, input_file.entry());
    a.bx(outp);

    ////////////////////////////////////////////////////////////////////////////
    // getnumber: decode variable length number
    // In:  tmp0 = Base context / 256. That is, call with tmp0 = 3 or tmp0 = 4.
    // Out: tmp1 = Number
    ////////////////////////////////////////////////////////////////////////////
    a.label("getnumber"s);
    a.push(getnumber_push_list);
    a.lsl(tmp0, tmp0, 8);
    a.mov(outp, 0);
    a.label("numberloop"s);
    a.add(outp, 2);
    a.mov(bitctx, tmp0);
    a.orr(bitctx, outp);
    a.bl("getbit"s);
    a.bcs("numberloop"s);
    a.mov(tmp1, 1);
    a.sub(outp, 1);
    a.label("bitsloop"s);
    a.mov(bitctx, tmp0);
    a.orr(bitctx, outp);
    a.bl("getbit"s);
    a.adc(tmp1, tmp1);
    a.sub(outp, 2);
    a.bcs("bitsloop"s);
    a.pop(to_pop_list(getnumber_push_list));

    ////////////////////////////////////////////////////////////////////////////
    // getkind: figure out whether the next symbol is a literal or a reference
    // In:  Nothing, sets up bitctx itself
    // Out: C = 0 if the next symbol is a literal, C = 1 if it is a reference
    //      bitctx = parity
    ////////////////////////////////////////////////////////////////////////////
    a.label("getkind"s);
    // Use parity as context : bitctx = (outp & 1) << 8
    a.lsl(bitctx, outp, 31);
    a.lsr(bitctx, bitctx, 23);
    // Fall through into getbit

    ////////////////////////////////////////////////////////////////////////////
    // getbit: decode a single bit
    // In:       bit context index in bitctx
    // Out:      Bit in C. That is, C=1 is a bit with value 1.
    // Destroys: bitctx
    // Keeps its state in bitbuf, isize and rvalue. So these must not be modified.
    ////////////////////////////////////////////////////////////////////////////
    a.label("getbit"s);
    a.push(getbit_push_list);
    a.b("loop_condition"s);
    a.label("readbit"s);
    a.lsl(bitbuf, bitbuf, 1);
    a.bne("nonewword"s);
    a.label("newword"s);
    a.ldr(bitbuf, inp, 0);
    a.add(inp, 4);
    // Shift data bit into C and make bit 0 the new sentinel bit.
    a.add(bitbuf, bitbuf);
    a.mov(tmp0, 1);
    a.orr(bitbuf, tmp0);
    a.label("nonewword"s);
    a.adc(rvalue, rvalue);
    a.add(isize, isize);
    a.label("loop_condition"s);
    a.lsr(tmp0, isize, 16);         // Loop while bit 15 is clear
    a.bcc("readbit"s);

    // Context is at : SP + CTX_TABLE_OFFSET + 2 * bitctx
    a.add(tmp0, bitctx, bitctx);
    a.add(tmp0, sp);

    // tmp1 = probability
    a.ldrh(tmp1, tmp0, CTX_TABLE_OFFSET);

    // outp = threshold = (intervalsize * prob) >> 16
    a.mov(outp, tmp1);
    a.mul(outp, isize);
    a.lsr(outp, outp, 16);

    // prob = prob - (prob >> ADJUST_SHIFT)
    a.lsr(bitctx, tmp1, ADJUST_SHIFT);
    a.sub(tmp1, tmp1, bitctx);

    a.sub(rvalue, rvalue, outp);
    a.bcc("one"s);
    a.label("zero"s);
    a.sub(isize, isize, outp);      // intervalsize -= threshold
    a.add(tmp1, 0);                 // C = 0, bit = 0
    a.b("done"s);
    a.label("one"s);
    a.mov(isize, outp);
    a.ldr(bitctx, 0xffff >> ADJUST_SHIFT);
    a.add(tmp1, bitctx);
    a.add(rvalue, outp);            // C = 1, bit = 1
    a.label("done"s);
    a.strh(tmp1, tmp0, CTX_TABLE_OFFSET);
    a.pop(to_pop_list(getbit_push_list));
    a.pool();

    ////////////////////////////////////////////////////////////////////////////
    // Compressed intro.
    // Must be word aligned, since the packer reads entire words from the bitstream.
    ////////////////////////////////////////////////////////////////////////////
    a.align(2);
    a.label("packed_intro"s);
    a.incbin(compressed_program.begin(), compressed_program.end());

    auto cart = a.link(0x08000000);
    assert(cart[ofs_fixed_byte] == 0x96);
    return cart;
}

}

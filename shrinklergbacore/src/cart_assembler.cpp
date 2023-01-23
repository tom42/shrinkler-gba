// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
//
// Code size (bytes)                Description. Note: code in header does not count towards size.
// --------------------------------------------------------------------------------------------------------------------
// 220                  n/a         Original prototype written in assembly. No code in header.
// 224                  59c796b     First version generated by this file. No code in header.
//                                  Using unoptimized ldr instructions to load load address and entry point from pool.

#include <bit>
#include <cstdint>
#include <string>
#include <type_traits>
#include "shrinklergbacore/cart_assembler.hpp"

namespace shrinklergbacore
{

using namespace lzasm::arm::arm32;
using namespace std::literals::string_literals;

constexpr size_t gba_header_size = 192;
constexpr auto initial_sp = 0x03007f00;

// GBA memory areas
constexpr uint32_t mem_bg_palette = 0x05000000;
constexpr uint32_t mem_vram = 0x06000000;

// GBA register addresses
constexpr uint32_t reg_base = 0x04000000;
constexpr uint32_t reg_dispcnt = reg_base + 0x00;

// DISPCNT bits
constexpr uint32_t MODE_4 = 4;
constexpr uint32_t BG2_ON = 1 << 10;

// Offsets in GBA cartridge header
constexpr size_t ofs_game_title = 0xa0;
constexpr size_t ofs_fixed_byte = 0xb2;
constexpr size_t ofs_device_type = 0xb4;
constexpr size_t ofs_game_version = 0xbc;
constexpr size_t ofs_complement = 0xbd;
constexpr size_t ofs_reserved2 = 0xbe;

template <typename T>
constexpr bool is_power_of_2(T n) noexcept
{
    static_assert(std::is_unsigned_v<T>, "T must be unsigned");
    return std::popcount(n) == 1;
}

constexpr uint32_t rgb5(uint32_t r, uint32_t g, uint32_t b) noexcept
{
    return r | (g << 5) | (b << 10);
}

constexpr uint32_t rgb8(uint32_t r, uint32_t g, uint32_t b) noexcept
{
    return rgb5(r >> 3, g >> 3, b >> 3);
}

cart_assembler::cart_assembler(const input_file& input_file, const std::vector<unsigned char>& compressed_program)
{
    // TODO: debug flag needs to come from command line
    m_data = assemble(input_file, compressed_program, false);
    write_complement();
}

void cart_assembler::write_complement()
{
    char complement = 0;
    for (size_t n = ofs_game_title; n < ofs_complement; ++n)
    {
        complement += m_data[n];
    }

    m_data[ofs_complement] = -(0x19 + complement);
}

std::vector<unsigned char> cart_assembler::assemble(const input_file& input_file, const std::vector<unsigned char>& compressed_program, bool debug)
{
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

    ////////////////////////////////////////////////////////////////////////////
    // Cartridge header
    ////////////////////////////////////////////////////////////////////////////

    arm_branch("code_start"s);
    // Logo
    byte(0x24, 0xff, 0xae, 0x51, 0x69, 0x9a, 0xa2, 0x21);
    byte(0x3d, 0x84, 0x82, 0x0a, 0x84, 0xe4, 0x09, 0xad);
    byte(0x11, 0x24, 0x8b, 0x98, 0xc0, 0x81, 0x7f, 0x21);
    byte(0xa3, 0x52, 0xbe, 0x19, 0x93, 0x09, 0xce, 0x20);
    byte(0x10, 0x46, 0x4a, 0x4a, 0xf8, 0x27, 0x31, 0xec);
    byte(0x58, 0xc7, 0xe8, 0x33, 0x82, 0xe3, 0xce, 0xbf);
    byte(0x85, 0xf4, 0xdf, 0x94, 0xce, 0x4b, 0x09, 0xc1);
    byte(0x94, 0x56, 0x8a, 0xc0, 0x13, 0x72, 0xa7, 0xfc);
    byte(0x9f, 0x84, 0x4d, 0x73, 0xa3, 0xca, 0x9a, 0x61);
    byte(0x58, 0x97, 0xa3, 0x27, 0xfc, 0x03, 0x98, 0x76);
    byte(0x23, 0x1d, 0xc7, 0x61, 0x03, 0x04, 0xae, 0x56);
    byte(0xbf, 0x38, 0x84, 0x00, 0x40, 0xa7, 0x0e, 0xfd);
    byte(0xff, 0x52, 0xfe, 0x03, 0x6f, 0x95, 0x30, 0xf1);
    byte(0x97, 0xfb, 0xc0, 0x85, 0x60, 0xd6, 0x80, 0x25);
    byte(0xa9, 0x63, 0xbe, 0x03, 0x01, 0x4e, 0x38, 0xe2);
    byte(0xf9, 0xa2, 0x34, 0xff, 0xbb, 0x3e, 0x03, 0x44);
    byte(0x78, 0x00, 0x90, 0xcb, 0x88, 0x11, 0x3a, 0x94);
    byte(0x65, 0xc0, 0x7c, 0x63, 0x87, 0xf0, 0x3c, 0xaf);
    byte(0xd6, 0x25, 0xe4, 0x8b, 0x38, 0x0a, 0xac, 0x72);
    byte(0x21, 0xd4, 0xf8, 0x07);
    // Game title (12 bytes), game code (4 bytes) and maker code (2 bytes).
    // These can be freely used, so we stick code into them.
    byte(0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    byte(0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    byte(0x00, 0x00, 0x00, 0x00);
    byte(0x00, 0x00);
    // Fixed byte of value 0x96, followed by unit code which can be freely chosen.
    byte(0x96);
    byte(0x00);
    // Device type (1 byte), followed by 7 unused bytes.
    byte(0x00);
    byte(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    // Game version (1 byte). Hard to make use of, since it's followed by the complement.
    byte(0x00);
    // Complement (will have to be fixed, so that checksum is 0)
    byte(0x00);
    // Checksum
    byte(0x00, 0x00);

    ////////////////////////////////////////////////////////////////////////////
    // Decompression code
    ////////////////////////////////////////////////////////////////////////////

    // 1536 contexts would be sufficient, but 2048 is smaller.
    //constexpr auto INIT_ONE_PROB = 0x8000;
    constexpr auto ADJUST_SHIFT = 4;
    constexpr auto SINGLE_BIT_CONTEXTS = 1;
    constexpr size_t NUM_CONTEXTS = 2048;

    constexpr auto getnumber_push_list = make_push_list(outp, lr);
    constexpr auto getbit_push_list = make_push_list(outp, tmp0, tmp1, lr);

    // Offset for accessing the context table relative to SP.
    constexpr auto CTX_TABLE_OFFSET = 4 * getbit_push_list.size() + 4 * getnumber_push_list.size() + 2 * SINGLE_BIT_CONTEXTS;

    align(2);
    // Initially the GBA is in ARM state. Switch to Thumb state first.
label("code_start"s);
    arm_to_thumb(inp);
    adr(inp, "packed_intro"s);

    // Initialize output pointer with load address of program.
    ldr(outp, input_file.load_address());

    // Initialize range decoder state.
    // rvalue will be set to 0 by the loop that follows.
    mov(isize, 1);
    lsl(bitbuf, isize, 31);

    // Initialize probabilities, writing two contexts per iteration.
    static_assert(is_power_of_2(NUM_CONTEXTS), "NUM_CONTEXTS must be a power of 2");
    lsl(rvalue, isize, std::countr_zero(NUM_CONTEXTS / 2)); // rvalue = NUM_CONTEXTS / 2
    lsl(bitctx, isize, 15);                                 // bitctx = (INIT_ONE_PROB << 16) | INIT_ONE_PROB
    orr(bitctx, bitbuf);
label("init"s);
    push(bitctx);
    sub(rvalue, 1);
    bne("init"s);
    // Now rvalue is 0

    // Main decompression loop.
label("literal"s);
    mov(tmp0, 1);
label("getlit"s);
    lsl(bitctx, outp, 31);        // bitctx = ((outp & 1) << 8) | current_byte
    lsr(bitctx, bitctx, 23);
    orr(bitctx, tmp0);
    bl("getbit"s);
    adc(tmp0, tmp0);
    cmp(tmp0, 255);
    bls("getlit"s);
    strb(tmp0, outp, 0);
    add(outp, 1);
label("switch"s);
    // After literal
    bl("getkind"s);
    bcc("literal"s);
    // Reference
    mov(bitctx, 0);               // bitctx = -1
    sub(bitctx, 1);
    bl("getbit"s);
    bcc("readoffset"s);
label("readlength"s);
    mov(tmp0, 4);
    bl("getnumber"s);
    mov(tmp0, offset);
    neg(tmp0, tmp0);
label("copyloop"s);
    ldrb(bitctx, outp, tmp0);
    strb(bitctx, outp, 0);
    add(outp, 1);
    sub(tmp1, 1);
    bne("copyloop"s);
    // After reference
    bl("getkind"s);
    bcc("literal"s);
label("readoffset"s);
    mov(tmp0, 3);
    bl("getnumber"s);
    sub(tmp1, 2);
    mov(offset, tmp1);
    bne("readlength"s);
label("donedecompressing"s);
    ldr(outp, input_file.entry());
    bx(outp);

    ////////////////////////////////////////////////////////////////////////////
    // getnumber: decode variable length number
    // In:  tmp0 = Base context / 256. That is, call with tmp0 = 3 or tmp0 = 4.
    // Out: tmp1 = Number
    ////////////////////////////////////////////////////////////////////////////
label("getnumber"s);
    push(getnumber_push_list);
    lsl(tmp0, tmp0, 8);
    mov(outp, 0);
label("numberloop"s);
    add(outp, 2);
    mov(bitctx, tmp0);
    orr(bitctx, outp);
    bl("getbit"s);
    bcs("numberloop"s);
    mov(tmp1, 1);
    sub(outp, 1);
label("bitsloop"s);
    mov(bitctx, tmp0);
    orr(bitctx, outp);
    bl("getbit"s);
    adc(tmp1, tmp1);
    sub(outp, 2);
    bcs("bitsloop"s);
    pop(to_pop_list(getnumber_push_list));

    ////////////////////////////////////////////////////////////////////////////
    // getkind: figure out whether the next symbol is a literal or a reference
    // In:  Nothing, sets up bitctx itself
    // Out: C = 0 if the next symbol is a literal, C = 1 if it is a reference
    //      bitctx = parity
    ////////////////////////////////////////////////////////////////////////////
label("getkind"s);
    // Use parity as context : bitctx = (outp & 1) << 8
    lsl(bitctx, outp, 31);
    lsr(bitctx, bitctx, 23);
    // Fall through into getbit

    ////////////////////////////////////////////////////////////////////////////
    // getbit: decode a single bit
    // In:       bit context index in bitctx
    // Out:      Bit in C. That is, C=1 is a bit with value 1.
    // Destroys: bitctx
    // Keeps its state in bitbuf, isize and rvalue. So these must not be modified.
    ////////////////////////////////////////////////////////////////////////////
label("getbit"s);
    push(getbit_push_list);
    b("loop_condition"s);
label("readbit"s);
    lsl(bitbuf, bitbuf, 1);
    bne("nonewword"s);
label("newword"s);
    ldr(bitbuf, inp, 0);
    add(inp, 4);
    // Shift data bit into C and make bit 0 the new sentinel bit.
    add(bitbuf, bitbuf);
    mov(tmp0, 1);
    orr(bitbuf, tmp0);
label("nonewword"s);
    adc(rvalue, rvalue);
    add(isize, isize);
label("loop_condition"s);
    lsr(tmp0, isize, 16);         // Loop while bit 15 is clear
    bcc("readbit"s);

    // Context is at : SP + CTX_TABLE_OFFSET + 2 * bitctx
    add(tmp0, bitctx, bitctx);
    add(tmp0, sp);

    // tmp1 = probability
    ldrh(tmp1, tmp0, CTX_TABLE_OFFSET);

    // outp = threshold = (intervalsize * prob) >> 16
    mov(outp, tmp1);
    mul(outp, isize);
    lsr(outp, outp, 16);

    // prob = prob - (prob >> ADJUST_SHIFT)
    lsr(bitctx, tmp1, ADJUST_SHIFT);
    sub(tmp1, tmp1, bitctx);

    sub(rvalue, rvalue, outp);
    bcc("one"s);
label("zero"s);
    sub(isize, isize, outp);      // intervalsize -= threshold
    add(tmp1, 0);                 // C = 0, bit = 0
    b("done"s);
label("one"s);
    mov(isize, outp);
    ldr(bitctx, 0xffff >> ADJUST_SHIFT);
    add(tmp1, bitctx);
    add(rvalue, outp);            // C = 1, bit = 1
label("done"s);
    strh(tmp1, tmp0, CTX_TABLE_OFFSET);
    pop(to_pop_list(getbit_push_list));
    pool();

    ////////////////////////////////////////////////////////////////////////////
    // Compressed intro.
    // Must be word aligned, since the packer reads entire words from the bitstream.
    ////////////////////////////////////////////////////////////////////////////
    align(2);
    m_depacker_size = current_lc() - gba_header_size;
label("packed_intro"s);
    incbin(compressed_program.begin(), compressed_program.end());
    debug_emit_panic_routine(debug);
    return link(0x08000000);
}

void cart_assembler::debug_call_panic_routine(bool debug, const char* message)
{
    if (!debug)
    {
        return;
    }

    adr(r2, "panic_message"s);
    bl("panic"s);

    // Embed panic message.
    // Note: the adr pseudo op requires word alignment.
    // TODO: if we call this more than once we get label redefinitions. Will worry about that later
    align(2);
label("panic_message"s);
    for (; *message; ++message)
    {
        byte(*message);
    }
    byte(0);

    // Align for subsequent thumb code.
    align(1);
}

void cart_assembler::debug_emit_panic_routine(bool debug)
{
    if (!debug)
    {
        return;
    }

    align(1);
label("panic"s);

    // Print panic message using Mappy / VisualBoyAdvance debug output.
    // r2 must point to a zero terminated string.
    ldr(r0, 0xc0ded00d);
    mov(r1, 0);
    and_(r0, r0);

    // Set video mode
    ldr(r0, MODE_4 | BG2_ON);
    ldr(r1, reg_dispcnt);
    str(r0, r1, 0);

    // Set palette
    // Set color 0 and 1 using a single 32 bit write
    ldr(r0, (rgb8(255, 255, 255) << 16) | rgb8(0, 119, 215));
    ldr(r1, mem_bg_palette);
    str(r0, r1, 0);

    // Draw sad face
    adr(r0, "sadface"s);
    ldr(r1, mem_vram + 4 * 240 + 4);
    mov(r4, 8);             // Copy 8 lines
label("copy_line"s);
    ldmia(!r0, r2 - r3);    // Read 8 pixels
    stmia(!r1, r2 - r3);    // Write 8 pixels
    add(r1, 240 - 8);       // Move to next line
    sub(r4, r4, 1);
    bne("copy_line"s);

    // Halt system
label("loop"s);
    b("loop"s);

    // 8x8 8bpp image of sad face
    align(2);
label("sadface"s);
    byte(0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00);
    byte(0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01);
    byte(0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01);
    byte(0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01);
    byte(0x01, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x01);
    byte(0x01, 0x00, 0x01, 0x00, 0x00, 0x01, 0x00, 0x01);
    byte(0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01);
    byte(0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00);
    pool();
}

}

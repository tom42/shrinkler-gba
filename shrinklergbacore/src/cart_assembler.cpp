// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
//
// Code size (bytes)                Description. Note: code in header does not count towards size.
// --------------------------------------------------------------------------------------------------------------------
// 220                  n/a         Original prototype written in assembly. Does not restore sp. No code in header.
// 224                  59c796b     First version generated by this file. Does not restore sp. No code in header.
//                                  Using unoptimized ldr instructions to load load address and entry point from pool.
// 228                  fe2a520     Restore sp to 0x03007f00 before executing unpacked intro. No code in header.
// 228                  e093da7     Code unlikely to vary is at the beginning of the ROM. No code in header.
// 200                  50f0d25     Moved code into header (getkind and the beginning of getbit)

#include <bit>
#include <cstdint>
#include <stdexcept>
#include <type_traits>
#include "fmt/core.h"
#include "shrinklergbacore/adler32.hpp"
#include "shrinklergbacore/cart_assembler.hpp"
#include "shrinklergbacore/complement.hpp"
#include "shrinklergbacore/gba.hpp"

namespace shrinklergbacore
{

using namespace lzasm::arm::arm32;
using namespace std::literals::string_literals;

constexpr auto initial_sp = 0x03007f00;

// GBA memory areas
constexpr uint32_t mem_bg_palette = 0x05000000;
constexpr uint32_t mem_vram = 0x06000000;
constexpr uint32_t mem_rom = 0x08000000;

// GBA register addresses
constexpr uint32_t reg_base = 0x04000000;
constexpr uint32_t reg_dispcnt = reg_base + 0x00;

// DISPCNT bits
constexpr uint32_t MODE_4 = 4;
constexpr uint32_t BG2_ON = 1 << 10;

constexpr auto fixed_byte_value = 0x96;

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
constexpr auto saved_sp = r9;           // Saved stack pointer

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

cart_assembler::cart_assembler(const input_file& input_file, const std::vector<unsigned char>& compressed_program, const depacker_settings& settings)
    : settings(settings)
{
    m_data = assemble(input_file, compressed_program);

    write_complement();

    throw_if_fixed_byte_wrong();
    throw_if_complement_wrong();
}

void cart_assembler::write_complement()
{
    // If we have no code in the header then we can use header fields normally and calculate and patch the complement field.
    //
    // If we do have code in the header, the complement field is part of a harmless bogus opcode that we inserted there.
    // It is a "mov rn,xx" instruction, where the complement field encodes the "mov rn" part. As destination register we
    // chose a register that we do not use and where we do not care that it gets clobbered.
    //
    // The game version field encodes the immediate value ("xx"), which we do not care about and which we can choose freely.
    // So we calculate a value for the game version field like we'd normally to for the complement field and then update
    // the game version field instead of the complement field.
    const size_t complement_byte_offset = settings.code_in_header ? ofs_game_version : ofs_complement;
    const size_t complement_byte_index = complement_byte_offset - ofs_game_title;
    m_data[ofs_game_title + complement_byte_index] = calculate_complement(&m_data[ofs_game_title], complement_byte_index);
}

std::vector<unsigned char> cart_assembler::assemble(const input_file& input_file, const std::vector<unsigned char>& compressed_program)
{
    // 1536 contexts would be sufficient, but 2048 is smaller.
    constexpr auto INIT_ONE_PROB = 0x8000u;
    constexpr auto ADJUST_SHIFT = 4;
    constexpr auto SINGLE_BIT_CONTEXTS = 1;
    constexpr auto NUM_CONTEXTS = 2048u;

    constexpr auto getnumber_push_list = make_push_list(outp, lr);
    constexpr auto getbit_push_list = make_push_list(outp, tmp0, tmp1, lr);

    // Offset for accessing the context table relative to SP.
    constexpr auto CTX_TABLE_OFFSET = 4 * getbit_push_list.size() + 4 * getnumber_push_list.size() + 2 * SINGLE_BIT_CONTEXTS;

    ////////////////////////////////////////////////////////////////////////////
    // Cartridge header
    ////////////////////////////////////////////////////////////////////////////

    arm_branch("code_start"s);
    emit_nintendo_logo();

    if (!settings.code_in_header)
    {
        emit_remaining_header();
    }

    // We're still inside the cartridge header, but most of the remaining fields
    // can be abused to stick code into them, so we'll just do that.

    ////////////////////////////////////////////////////////////////////////////
    // getkind: figure out whether the next symbol is a literal or a reference
    // In:  Nothing, sets up bitctx itself
    // Out: C = 0 if the next symbol is a literal, C = 1 if it is a reference
    //      bitctx = parity
    ////////////////////////////////////////////////////////////////////////////
    throw_if_not_aligned(1);
    align(1);
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
    if (settings.code_in_header)
    {
        // Fixed byte of value 0x96, followed by unit code which can be freely chosen.
        // We insert an instruction here that does not bother us and stomp over it.
        throw_if_wrong_lc(ofs_fixed_byte, "fixed byte");
        mov(tmp0, fixed_byte_value);
    }
    mov(tmp0, 1);
    orr(bitbuf, tmp0);
label("nonewword"s);
    adc(rvalue, rvalue);
    add(isize, isize);
label("loop_condition"s);
    if (settings.code_in_header)
    {
        // game version (immediate value), followed by complement (opcode).
        // Again, insert an instruction that does not bother us and stomp over it.
        // Later we need to fix up the immediate value (the game version),
        // so that it matches the complement. We cannot fix up the complement,
        // like we'd normally do, since we'd end up with a random, potentially
        // harmful instruction. See also write_complement().
        constexpr unsigned char dummy_game_version_that_gets_overwritten_later_instead_of_the_complement = 0x77;
        throw_if_wrong_lc(ofs_game_version, "game version");
        mov(tmp0, dummy_game_version_that_gets_overwritten_later_instead_of_the_complement);
    }
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

    ////////////////////////////////////////////////////////////////////////////
    // Main decompression code
    ////////////////////////////////////////////////////////////////////////////

    // Entry point. Initially the GBA is in ARM state.
    // Immediately switch to Thumb state and save sp.
    throw_if_not_aligned(2);
    align(2);
label("code_start"s);
    arm_to_thumb(inp);
    mov(saved_sp, sp);

    // Initialize input and output pointers.
    adr(inp, "packed_intro"s);
    ldr(outp, input_file.load_address());

    // Initialize range decoder state.
    // rvalue will be set to 0 by the loop that follows.
    mov(isize, 1);
    lsl(bitbuf, isize, 31);

    // Initialize probabilities, writing two contexts per iteration.
    static_assert(is_power_of_2(NUM_CONTEXTS), "NUM_CONTEXTS must be a power of 2");
    static_assert(is_power_of_2(INIT_ONE_PROB), "INIT_ONE_PROB must be a power of 2");
    lsl(rvalue, isize, std::countr_zero(NUM_CONTEXTS / 2)); // rvalue = NUM_CONTEXTS / 2
    lsl(bitctx, isize, std::countr_zero(INIT_ONE_PROB));    // bitctx = (INIT_ONE_PROB << 16) | INIT_ONE_PROB
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
    mov(sp, saved_sp);
    debug_check_decompressed_data_size(input_file);
    debug_check_decompressed_data(input_file);
    debug_check_sp_on_exit();
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
    pool();

    ////////////////////////////////////////////////////////////////////////////
    // Compressed intro.
    // Must be word aligned, since
    // * The packer reads entire words from the bitstream.
    // * We use adr to load the address to the packed data,
    //   which requires word alignment.
    ////////////////////////////////////////////////////////////////////////////
    align(2);
    m_depacker_size = current_lc() - gba_header_size;
label("packed_intro"s);
    incbin(compressed_program.begin(), compressed_program.end());
    debug_emit_panic_routine();
    return link(mem_rom);
}

void cart_assembler::emit_nintendo_logo()
{
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
}

void cart_assembler::emit_remaining_header()
{
    // Game title (12 bytes), game code (4 bytes) and maker code (2 bytes)
    throw_if_wrong_lc(ofs_game_title, "game title");
    byte(0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    byte(0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
    byte(0x00, 0x00, 0x00, 0x00);
    byte(0x00, 0x00);

    // Fixed byte of value 0x96, followed by unit code
    throw_if_wrong_lc(ofs_fixed_byte, "fixed byte");
    byte(fixed_byte_value);
    byte(0x00);

    // Device type (1 byte), followed by 7 unused bytes
    throw_if_wrong_lc(ofs_device_type, "device type");
    byte(0x00);
    byte(0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);

    // Game version (1 byte)
    throw_if_wrong_lc(ofs_game_version, "game version");
    byte(0x00);

    // Complement (will have to be fixed, so that checksum is 0)
    throw_if_wrong_lc(ofs_complement, "complement");
    byte(0x00);

    // Reserved area
    throw_if_wrong_lc(ofs_reserved2, "reserved area 2");
    byte(0x00, 0x00);
}

void cart_assembler::asciz(const std::string& s)
{
    for (char c : s)
    {
        byte(c);
    }

    byte(0);
}

void cart_assembler::debug_check_decompressed_data_size(const input_file& input_file)
{
    if (!settings.debug_checks)
    {
        return;
    }

    // outp = outp - load_address = actual number of bytes depacked
    ldr(tmp1, input_file.load_address());
    sub(outp, outp, tmp1);

    // Compare actual with expected number of bytes depacked
    ldr(tmp1, input_file.loaded_data_size());
    cmp(outp, tmp1);
    beq("decompressed_data_size_ok"s);
    debug_call_panic_routine("Wrong decompressed data size");
label("decompressed_data_size_ok"s);
}

void cart_assembler::debug_check_decompressed_data(const input_file& input_file)
{
    if (!settings.debug_checks)
    {
        return;
    }

    constexpr auto decompressed_data = r0;
    constexpr auto s1 = r1;
    constexpr auto s2 = r2;
    constexpr auto loop_counter = r3;
    constexpr auto byte = r4;
    constexpr auto base = r5;
    constexpr auto expected_checksum = r6;

    // Calculate adler32 checksum of depacked data
    ldr(base, 65521);
    ldr(decompressed_data, input_file.load_address());
    mov(s1, 1);
    mov(s2, 0);
    ldr(loop_counter, input_file.loaded_data_size());
label("adler32_loop"s);
    ldrb(byte, decompressed_data, 0);
    add(s1, byte);
    cmp(s1, base);
    bcc("s1_ok"s);
    sub(s1, s1, base);
label("s1_ok"s);
    add(s2, s1);
    cmp(s2, base);
    bcc("s2_ok"s);
    sub(s2, s2, base);
label("s2_ok"s);
    add(decompressed_data, 1);
    sub(loop_counter, 1);
    bne("adler32_loop"s);
    lsl(s2, s2, 16);
    orr(s1, s2);

    // Compare expected and actual checksum
    ldr(expected_checksum, adler32(input_file.data()));
    cmp(s1, expected_checksum);
    beq("checksum_ok"s);
    debug_call_panic_routine("Wrong decompressed data checksum");
label("checksum_ok"s);
}

void cart_assembler::debug_check_sp_on_exit()
{
    if (!settings.debug_checks)
    {
        return;
    }

    ldr(r0, initial_sp);
    cmp(r0, sp);
    beq("sp_ok"s);
    debug_call_panic_routine("Wrong sp after depacking\n");
label("sp_ok"s);
}

void cart_assembler::debug_call_panic_routine(const std::string& message)
{
    if (!settings.debug_checks)
    {
        return;
    }

    adr(r2, message);
    bl("panic"s);

    // Embed panic message.
    // Note 1: the adr pseudo op requires word alignment.
    // Note 2: since lzasm does not support local labels we use the message itself
    //         also as label name. This is obviously totally awful, but works
    //         as long as the macro is not called twice with the same message.
    align(2);
label(message);
    asciz(message);

    // Align for subsequent thumb code.
    align(1);
}

void cart_assembler::debug_emit_panic_routine()
{
    if (!settings.debug_checks)
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

void cart_assembler::throw_if_wrong_lc(lzasm::arm::arm32::address_t expected_lc, const char* what) const
{
    if (current_lc() != expected_lc)
    {
        throw std::runtime_error(fmt::format("INTERNAL ERROR: {} is at wrong offset. Expected it to be at {:#x}, but it is at {:#x}", what, expected_lc, current_lc()));
    }
}

void cart_assembler::throw_if_not_aligned(lzasm::arm::arm32::address_t alignment) const
{
    auto byte_alignment = 1u << alignment;

    if (current_lc() % byte_alignment)
    {
        throw std::runtime_error(fmt::format("INTERNAL ERROR: Location counter is not aligned to {} bytes. We're wasting space", byte_alignment));
    }
}

void cart_assembler::throw_if_fixed_byte_wrong() const
{
    auto actual_byte = m_data.at(ofs_fixed_byte);
    if (actual_byte != fixed_byte_value)
    {
        throw std::runtime_error(fmt::format("INTERNAL ERROR: Fixed byte at {:#x} has wrong value. Should be {:#x}, but is {:#x}", ofs_fixed_byte, fixed_byte_value, actual_byte));
    }
}

void cart_assembler::throw_if_complement_wrong() const
{
    auto expected_complement = calculate_complement(&m_data[ofs_game_title]);
    auto actual_complement = m_data.at(ofs_complement);
    if (actual_complement != expected_complement)
    {
        throw std::runtime_error(fmt::format("INTERNAL ERROR: Complement at {:#x} has wrong value. Should be {:#x}, but is {:#x}", ofs_complement, expected_complement, actual_complement));
    }
}

}

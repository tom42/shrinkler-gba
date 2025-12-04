// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <concepts>
#include <cstdint>
#include <lzasm/arm/arm32/divided_thumb_assembler.hpp>
#include <string>

module shrinkler_gba;

namespace shrinkler_gba
{

using namespace lzasm::arm::arm32;
using namespace std::literals::string_literals;

namespace
{

constexpr auto initial_sp = 0x03007f00;

// GBA memory areas
constexpr uint32_t mem_bg_palette = 0x05000000;
constexpr uint32_t mem_vram = 0x06000000;
constexpr uint32_t mem_rom = 0x08000000;

// GBA register addresses
constexpr uint32_t reg_base = 0x04000000;
constexpr uint32_t reg_dispcnt = reg_base + 0x00;

// DISPCNT bits
constexpr uint32_t mode_4 = 4;
constexpr uint32_t bg2_on = 1 << 10;

constexpr auto fixed_byte_value = 0x96;

// Register aliases
constexpr auto inp = r0;        // Compressed data
constexpr auto outp = r1;       // Decompressed data
constexpr auto tmp0 = r2;       // Scratch register 0
constexpr auto tmp1 = r3;       // Scratch register 1
constexpr auto rvalue = r4;     // Range value
constexpr auto isize = r5;      // Interval size
constexpr auto bitbuf = r6;     // Input bit buffer
constexpr auto bitctx = r7;     // Bit context index
constexpr auto offset = r8;     // Offset
constexpr auto saved_sp = r9;   // Saved stack pointer

constexpr bool is_power_of_2(std::unsigned_integral auto n)
{
    return std::popcount(n) == 1;
}

constexpr uint32_t rgb5(uint32_t r, uint32_t g, uint32_t b)
{
    return r | (g << 5) | (b << 10);
}

constexpr uint32_t rgb8(uint32_t r, uint32_t g, uint32_t b)
{
    return rgb5(r >> 3, g >> 3, b >> 3);
}

}

}

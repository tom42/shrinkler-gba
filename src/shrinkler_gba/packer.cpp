// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <format>
#include <iostream>
#include <lzasm/arm/arm32/divided_thumb_assembler.hpp>

module shrinkler_gba;

namespace shrinkler_gba
{

void cartridge_assembler::emit_nintendo_logo()
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

void cartridge_assembler::emit_remaining_header()
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

void cartridge_assembler::write_complement(std::vector<unsigned char>& cartridge_data, const size_t complement_byte_offset)
{
    // TODO: move this comment elsewhere or reformulate it: like that it does not make sense here
    // If we have no code in the header then we can use header fields normally and calculate and patch the complement field.
    //
    // If we do have code in the header, the complement field is part of a harmless bogus opcode that we inserted there.
    // It is a "mov rn,xx" instruction, where the complement field encodes the "mov rn" part. As destination register we
    // choose a register that we do not use and where we do not care that it gets clobbered.
    //
    // The game version field encodes the immediate value ("xx"), which we do not care about and which we can choose freely.
    // So we calculate a value for the game version field like we'd normally to for the complement field and then update
    // the game version field instead of the complement field.
    const size_t complement_byte_index = complement_byte_offset - ofs_game_title;
    unsigned char complement = calculate_complement(&cartridge_data[ofs_game_title], complement_byte_index);
    cartridge_data[ofs_game_title + complement_byte_index] = complement;
}

void cartridge_assembler::throw_if_wrong_lc(lzasm::arm::arm32::address_t expected_lc, const char* what) const
{
    if (current_lc() != expected_lc)
    {
        throw std::runtime_error(std::format("INTERNAL ERROR: {} is at wrong offset. Expected it to be at {:#x}, but it is at {:#x}", what, expected_lc, current_lc()));
    }
}

void cartridge_assembler::throw_if_not_aligned(lzasm::arm::arm32::address_t alignment) const
{
    auto byte_alignment = 1u << alignment;

    if (current_lc() % byte_alignment)
    {
        throw std::runtime_error(std::format("INTERNAL ERROR: Location counter is not aligned to {} bytes. We're wasting space", byte_alignment));
    }
}

void cartridge_assembler::throw_if_fixed_byte_wrong(const std::vector<unsigned char>& cartridge_data) const
{
    auto actual_byte = cartridge_data.at(ofs_fixed_byte);
    if (actual_byte != fixed_byte_value)
    {
        throw std::runtime_error(std::format("INTERNAL ERROR: Fixed byte at {:#x} has wrong value. Should be {:#x}, but is {:#x}", ofs_fixed_byte, fixed_byte_value, actual_byte));
    }
}

void cartridge_assembler::throw_if_complement_wrong(const std::vector<unsigned char>& cartridge_data) const
{
    // TODO: should we do a size check of cartridge_data here, so that we don't buffer overflow when calculating the complement?

    auto expected_complement = calculate_complement(&cartridge_data[ofs_game_title]);
    auto actual_complement = cartridge_data.at(ofs_complement);
    if (actual_complement != expected_complement)
    {
        throw std::runtime_error(std::format("INTERNAL ERROR: Complement at {:#x} has wrong value. Should be {:#x}, but is {:#x}", ofs_complement, expected_complement, actual_complement));
    }
}

}

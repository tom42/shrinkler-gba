// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstddef>
#include <lzasm/arm/arm32/divided_thumb_assembler.hpp>
#include <string>
#include <vector>

export module shrinkler_gba:packer;
import :input_file;
import :utility;

namespace shrinkler_gba
{

struct cartridge final
{
    std::string packer;
    bytevector data;
    size_t compressed_size = 0;
    size_t depacker_size = 0;
    std::vector<std::string> info;
};

class packer
{
public:
    virtual ~packer() = default;

    virtual cartridge pack(const input_file& input_file) = 0;
};

class cartridge_assembler : protected lzasm::arm::arm32::divided_thumb_assembler
{
public:
    void emit_nintendo_logo();
    void emit_remaining_header();

    virtual ~cartridge_assembler() override = default;

    void write_complement(bytevector& cartridge_data, const size_t complement_byte_offset);

    void throw_if_wrong_lc(lzasm::arm::arm32::address_t expected_lc, const char* what) const;
    void throw_if_not_aligned(lzasm::arm::arm32::address_t alignment) const;
    void throw_if_fixed_byte_wrong(const bytevector& cartridge_data) const;
    void throw_if_complement_wrong(const bytevector& cartridge_data) const;

protected:
    cartridge_assembler() = default;
    cartridge_assembler(const cartridge_assembler&) = default;
};

}

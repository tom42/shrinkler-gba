// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstddef>
#include <lzasm/arm/arm32/divided_thumb_assembler.hpp>
#include <vector>

export module shrinkler_gba:packer;
import :input_file;

namespace shrinkler_gba
{

struct cartridge final
{
    std::vector<unsigned char> data;
    size_t compressed_size = 0;
    size_t depacker_size = 0;
};

class packer
{
public:
    virtual ~packer() = default;

    virtual cartridge pack(const input_file& input_file) = 0;
};

// TODO: public, protected or private inheritance
// TODO: this is a mixin class I think? How do we forbid instantiation
class cartridge_assembler : public lzasm::arm::arm32::divided_thumb_assembler
{
public:
    void emit_nintendo_logo();

    virtual ~cartridge_assembler() = default;
};

}

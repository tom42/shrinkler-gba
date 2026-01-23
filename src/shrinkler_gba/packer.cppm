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

}

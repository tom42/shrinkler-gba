// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <lzasm/arm/arm32/divided_thumb_assembler.hpp>
#include <vector>

export module shrinkler_gba:shrinkler_cartridge_assembler;
import :input_file;

namespace shrinkler_gba
{

class shrinkler_depacker_settings final
{
public:
    bool code_in_header = true;
    bool debug_checks = false;
};

class shrinkler_cartridge_assembler final : private lzasm::arm::arm32::divided_thumb_assembler
{
public:
    shrinkler_cartridge_assembler(const input_file& input_file, const std::vector<unsigned char>& compressed_program, const shrinkler_depacker_settings& settings);

    // TODO: return by const reference (once we're not returning a temporary anymore)
    std::vector<unsigned char> data() const
    {
        // TODO: real return value
        return {};
    }

private:
};

}

// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_CART_ASSEMBLER_HPP
#define SHRINKLERGBACORE_CART_ASSEMBLER_HPP

#include <vector>
#include "lzasm/arm/arm32/divided_thumb_assembler.hpp"
#include "shrinklergbacore/input_file.hpp"

namespace shrinklergbacore
{

// TODO: stick code into header, but turn it around first so that uninteresting bits are there That is:
//       * Turn around depacker code, so that the non-changing bits are at the beginning (AND CHECK THIS IN)
//       * Then move these bits into the header.
// TODO: assemble stuff in ctor again? Rationale: calling assmble() multiple times is not good.
class cart_assembler final : private lzasm::arm::arm32::divided_thumb_assembler
{
public:
    std::vector<unsigned char> assemble(const input_file& input_file, const std::vector<unsigned char>& compressed_program);
};

}

#endif

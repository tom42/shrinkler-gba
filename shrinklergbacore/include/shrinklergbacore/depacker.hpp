// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_DEPACKER_HPP
#define SHRINKLERGBACORE_DEPACKER_HPP

#include "lzasm/arm/arm32/divided_thumb_assembler.hpp"

namespace shrinklergbacore
{

class depacker final : private lzasm::arm::arm32::divided_thumb_assembler
{
public:
    depacker();
private:
    void assemble();
};

}

#endif

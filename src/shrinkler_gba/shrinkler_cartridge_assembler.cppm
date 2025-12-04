// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <vector>
#include "lzasm/arm/arm32/divided_thumb_assembler.hpp"

export module shrinkler_gba:shrinkler_cartridge_assembler;

namespace shrinkler_gba
{

class shrinkler_cartridge_assembler final : private lzasm::arm::arm32::divided_thumb_assembler
{
public:
	// TODO: return by const reference (once we're not returning a temporary anymore)
	std::vector<unsigned char> data() const
	{
		// TODO: real return value
		return {};
	}

private:
};

}

// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBACORE_COMPLEMENT_HPP
#define SHRINKLERGBACORE_COMPLEMENT_HPP

#include <vector>

namespace shrinklergbacore
{

// TODO: document
// TODO: unit test would not hurt either...
// TODO: create generalized version that allows an arbitrary variable byte
unsigned char calculate_complement(const std::vector<unsigned char>& header_data);

}

#endif

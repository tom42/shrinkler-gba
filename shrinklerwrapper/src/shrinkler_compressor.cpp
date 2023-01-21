// SPDX-FileCopyrightText: 2023 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#include "shrinklerwrapper/shrinklerwrapper.hpp"
#include "shrinkler_compressor_impl.hpp"

namespace shrinklerwrapper
{

std::vector<unsigned char> shrinkler_compressor::compress(const std::vector<unsigned char>& data) const
{
    detail::shrinkler_compressor_impl compressor(parameters);
    return compressor.compress(data);
}

}

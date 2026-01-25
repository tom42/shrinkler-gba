// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <span>

module shrinkler_gba;

namespace shrinkler_gba
{

namespace
{

constinit const packer_info packers[]
{
    { "lzss", nullptr }, // TODO: creation function for lzss packer
    { "shrinkler", nullptr } // TODO: creation function for shrinkler packer
};

}

namespace packer_registry
{

std::span<const packer_info> all_info()
{
    return packers;
}

}

}

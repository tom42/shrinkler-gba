// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module shrinkler_gba;

namespace shrinkler_gba
{

namespace
{

constinit const packer_info packers[]
{
    { "lzss" },
    { "shrinkler" }
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

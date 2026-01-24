// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <vector>

module shrinkler_gba;

namespace shrinkler_gba
{

namespace
{

constinit const std::vector<packer_info> packers
{
    { "lzss" },
    { "shrinkler" }
};

}

namespace packer_registry
{

const std::vector<packer_info>& all_packers()
{
    return packers;
}

}

}

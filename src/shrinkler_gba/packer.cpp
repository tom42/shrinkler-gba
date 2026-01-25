// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <memory>
#include <ranges>
#include <span>
#include <vector>

module shrinkler_gba;

namespace shrinkler_gba
{

namespace
{

std::unique_ptr<packer> create_lzss_packer()
{
    return std::make_unique<lzss_packer>();
}

std::unique_ptr<packer> create_shrinkler_packer()
{
    return std::make_unique<shrinkler_packer>();
}

constinit const packer_info packers[]
{
    { "lzss", create_lzss_packer },
    { "shrinkler", create_shrinkler_packer }
};

}

namespace packer_registry
{

std::span<const packer_info> all_info()
{
    return packers;
}

std::vector<std::unique_ptr<packer>> create_all_packers()
{
    return all_info()
        | std::views::transform([](const auto& packer_info) { return packer_info.create(); })
        | std::ranges::to<std::vector<std::unique_ptr<packer>>>();
}

}

}

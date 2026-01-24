// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstddef>
#include <string>
#include <vector>

export module shrinkler_gba:packer;
import :input_file;
import :utility;

namespace shrinkler_gba
{

struct cartridge final
{
    std::string packer;
    bytevector data;
    size_t compressed_size = 0;
    size_t depacker_size = 0;
    std::vector<std::string> info;
};

struct packer_info final
{
    constexpr packer_info(const char* name) : m_name(name) {}

    const char* m_name;
};

class packer
{
public:
    virtual ~packer() = default;

    virtual cartridge pack(const input_file& input_file) = 0;
};

namespace packer_registry
{

// TODO: does this need a test?
// TODO: return all packer infos, sorted alphabetically
const std::vector<packer_info>& all_packers();

}

}

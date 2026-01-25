// SPDX-FileCopyrightText: 2025 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstddef>
#include <memory>
#include <span>
#include <string>
#include <vector>

export module shrinkler_gba:packer;
import :input_file;
import :options;
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

class packer
{
public:
    virtual ~packer() = default;

    virtual cartridge pack(const input_file& input_file) = 0;
};

struct packer_info final
{
    const char* name;
    std::unique_ptr<packer>(*create)(const options& options);
};

namespace packer_registry
{

// TODO: does this need a test?
// TODO: return all packer infos, sorted alphabetically
std::span<packer_info> all_packers();

}

}

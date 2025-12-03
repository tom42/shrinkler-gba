// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstdint>
#include <elfio/elfio.hpp>
#include <gsl/gsl>
#include <vector>

export module shrinkler_gba:input_file;

namespace shrinkler_gba
{

bool is_section_included(gsl::not_null<const ELFIO::section*> s);

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
class input_file final
{
public:
    input_file(const ELFIO::elfio& elfio);

    uint32_t entry() const { return m_entry; }

    bool is_thumb_entry() const { return entry() & 1; }

    uint32_t load_address() const { return m_load_address; }

    uint32_t loaded_data_size() const { return gsl::narrow<uint32_t>(m_data.size()); }

    const std::vector<unsigned char>& data() const { return m_data; }

private:
    void read_entry(const ELFIO::elfio& elfio);
    void load_binary(const ELFIO::elfio& elfio);

    uint32_t m_entry = 0;
    uint32_t m_load_address = 0;
    std::vector<unsigned char> m_data;
};

}

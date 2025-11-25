// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

export module shrinkler_gba:input_file;
import :console;

namespace shrinkler_gba
{

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
class input_file final
{
public:
    void load(const std::string& path, const console& console);

    void load(std::istream& stream, const console& console);

    uint32_t entry() const { return m_entry; }

    bool is_thumb_entry() const { return entry() & 1; }

    uint32_t load_address() const { return m_load_address; }

    // TODO: have some sort of narrowing cast that performs a check at runtime?
    uint32_t loaded_data_size() const { return static_cast<uint32_t>(m_data.size()); }

    const std::vector<unsigned char>& data() const { return m_data; }

private:
    void load_elf(std::istream& stream);
    void reset();

    uint32_t m_entry = 0;
    uint32_t m_load_address = 0;
    std::vector<unsigned char> m_data;
};

}

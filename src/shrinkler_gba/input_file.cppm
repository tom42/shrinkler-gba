// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstdint>
#include <iosfwd>
#include <string>
#include <vector>

export module shrinkler_gba:input_file;

namespace shrinkler_gba
{

SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
class input_file final
{
public:
    void load(const std::string& path);

    void load(std::istream& stream);

    uint32_t entry() const { return m_entry; }

    bool is_thumb_entry() const { return entry() & 1; }

    uint32_t load_address() const { return m_load_address; }

    uint32_t loaded_data_size() const { return 0; } // TODO: real implementation

    const std::vector<unsigned char>& data() const { return m_data; }

private:
    void load_elf(std::istream& stream);
    void reset();

    uint32_t m_entry = 0;
    uint32_t m_load_address = 0;
    std::vector<unsigned char> m_data;
};

}

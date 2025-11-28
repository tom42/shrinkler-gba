// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstdint>
#include <gsl/gsl>
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
    static input_file load(std::istream& stream, const console& console);

    static input_file load(const std::string& path, const console& console);

    uint32_t entry() const { return m_entry; }

    bool is_thumb_entry() const { return entry() & 1; }

    uint32_t load_address() const { return m_load_address; }

    uint32_t loaded_data_size() const { return gsl::narrow<uint32_t>(m_data.size()); }

    const std::vector<unsigned char>& data() const { return m_data; }

private:
    input_file(std::istream& stream, const console& console);

    uint32_t m_entry = 0;
    uint32_t m_load_address = 0;
    std::vector<unsigned char> m_data;
};

}

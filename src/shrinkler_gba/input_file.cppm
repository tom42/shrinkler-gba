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

// TODO: this was kind of better before we refactored it
//       * Turn it back as it was, with most methods being either instance or class methods
//         * The static factory methods seem kind of pointless. Turn them into two constructors, the private constructor can go
//       * Still, do the loading of the ELF file in the constructor, so that there is no load() method, and reset() is not needed
//       * Since we can have instance methods, the output parameter in convert_to_binary is not needed anymore
//       * Obviously, input_file IS violating SRP:
//         * It dumps all sorts of data that might be interesting for debugging
//         * It loads an ELF file and converts it to binary
//         * Think about separating these two concerns
//           * There is one bit of code I think we're sharing: is_section_included() => Well, no problem, we factor out that method
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

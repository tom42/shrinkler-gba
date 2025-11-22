// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module;

#include <cstdint>
#include <vector>

export module shrinkler_gba:input_file;

namespace shrinkler_gba
{

// TODO: get ELFIO, but can we organize things so that no ELFIO header is used in .cppm files?
//       => Note: back then we used ELFIO 3.9, but we should probably migrate to the latest version, that would be 3.12
//       => Note: output from vcpkg install elfio:
//            # this is heuristically generated, and may not be correct
//            find_package(elfio CONFIG REQUIRED) (ugh: what is CONFIG?)
//            target_link_libraries(main PRIVATE elfio::elfio)
SHRINKLER_GBA_EXPORT_FOR_UNIT_TESTING
class input_file final
{
public:
    uint32_t entry() const { return m_entry; }

    uint32_t load_address() const { return m_load_address; }

    uint32_t loaded_data_size() const { return 0; } // TODO: real implementation

    const std::vector<unsigned char>& data() const { return m_data; }

private:
    uint32_t m_entry = 0;
    uint32_t m_load_address = 0;
    std::vector<unsigned char> m_data;
};

}

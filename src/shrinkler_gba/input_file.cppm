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
private:
    uint32_t m_entry = 0;
    uint32_t m_load_address = 0;
    std::vector<unsigned char> m_data;
};

}

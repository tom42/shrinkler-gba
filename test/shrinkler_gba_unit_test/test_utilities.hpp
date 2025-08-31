// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#ifndef SHRINKLER_GBA_UNIT_TEST_TEST_UTILITIES_HPP
#define SHRINKLER_GBA_UNIT_TEST_TEST_UTILITIES_HPP

#include <string>
#include <vector>

namespace shrinkler_gba_unit_test
{

// TODO: consider turning this into a module?
std::vector<unsigned char> load_binary_file(const std::string& filename);
const std::vector<unsigned char> make_bytevector(const char* s);

}

#endif

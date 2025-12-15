// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

#ifndef SHRINKLER_GBA_UNIT_TEST_TEST_UTILITIES_HPP
#define SHRINKLER_GBA_UNIT_TEST_TEST_UTILITIES_HPP

#include <string>
#include <vector>

import shrinkler_gba;

namespace shrinkler_gba_unit_test
{

shrinkler_gba::bytevector load_binary_file(const std::string& filename);
shrinkler_gba::bytevector make_bytevector(const char* s);

}

#endif

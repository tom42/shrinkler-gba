// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT
// shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA

#ifndef SHRINKLERGBA_UNITTEST_TEST_UTILITIES_HPP
#define SHRINKLERGBA_UNITTEST_TEST_UTILITIES_HPP

#include <filesystem>
#include <vector>

#define CHECK_EXCEPTION(S, E, M)                                        \
    BOOST_CHECK_EXCEPTION(                                              \
        S,                                                              \
        E,                                                              \
        [](const auto& e) { BOOST_TEST(e.what() == M); return true; });

namespace shrinklergba_unittest
{

std::vector<unsigned char> load_binary_file(const std::filesystem::path& filename);
const std::vector<unsigned char> make_bytevector(const char* s);

}

#endif

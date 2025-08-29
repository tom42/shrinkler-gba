# SPDX-FileCopyrightText: 2025 Thomas Mathys
# SPDX-License-Identifier: MIT

# TODO: add a bogus source, so we can get something build
# TODO: link to shrinkler_gba library (unit testing version, need to build that first)
# TODO: once it is working, remove test_the_test.cpp

add_executable(shrinkler_gba_unit_test test_the_test.cpp)
vtg_target_enable_warnings(shrinkler_gba_unit_test)
target_link_libraries(shrinkler_gba_unit_test PRIVATE Catch2::Catch2WithMain)
catch_discover_tests(shrinkler_gba_unit_test)

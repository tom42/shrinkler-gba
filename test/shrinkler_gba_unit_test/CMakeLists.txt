# SPDX-FileCopyrightText: 2025 Thomas Mathys
# SPDX-License-Identifier: MIT

# TODO: add a bogus source, so we can get something build
# TODO: once it is working, remove test_the_test.cpp

add_executable(
  shrinkler_gba_unit_test
  adler32_test.cpp
  test_the_test.cpp)

vtg_target_enable_warnings(shrinkler_gba_unit_test)
target_link_libraries(shrinkler_gba_unit_test PRIVATE shrinkler_gba_unit_testing Catch2::Catch2WithMain)
catch_discover_tests(shrinkler_gba_unit_test)

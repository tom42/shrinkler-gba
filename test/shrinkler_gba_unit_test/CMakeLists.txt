# SPDX-FileCopyrightText: 2025 Thomas Mathys
# SPDX-License-Identifier: MIT

# Generate configuration header
set(SHRINKLER_GBA_UNIT_TEST_TESTDATA_DIRECTORY "${PROJECT_SOURCE_DIR}/test/shrinkler_gba_unit_test/testdata")
configure_file(
  shrinkler_gba_unit_test_config.hpp.in
  "${CMAKE_CURRENT_BINARY_DIR}/shrinkler_gba_unit_test_config.hpp")

add_executable(
  shrinkler_gba_unit_test
  adler32_test.cpp
  complement_test.cpp
  console_test.cpp
  elf_info_test.cpp
  input_file_test.cpp
  options_test.cpp
  pack_test.cpp
  parse_command_line_test.cpp
  table_printer_test.cpp
  test_utilities.cpp)

vtg_target_enable_warnings(shrinkler_gba_unit_test)
target_include_directories(shrinkler_gba_unit_test PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_libraries(shrinkler_gba_unit_test PRIVATE shrinkler_gba_unit_testing Catch2::Catch2WithMain elfio::elfio Microsoft.GSL::GSL)
catch_discover_tests(shrinkler_gba_unit_test)

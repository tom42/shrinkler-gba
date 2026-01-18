# SPDX-FileCopyrightText: 2026 Thomas Mathys
# SPDX-License-Identifier: MIT

# TODO: implement test:
#       * Compare the output file

function(make_test input_file options)
  add_test(
    NAME "Compress ${input_file}"
    COMMAND shrinkler-gba "${CMAKE_CURRENT_SOURCE_DIR}/lostmarbles.elf" ${options} -o "${CMAKE_CURRENT_BINARY_DIR}/lostmarbles.elf.actual")
  add_test(
    NAME "Verify ${input_file}"
    COMMAND ${CMAKE_COMMAND} -E compare_files "${CMAKE_CURRENT_SOURCE_DIR}/lostmarbles.elf.expected" "${CMAKE_CURRENT_BINARY_DIR}/lostmarbles.elf.actual")
endfunction()

make_test(lostmarbles.elf -p9)

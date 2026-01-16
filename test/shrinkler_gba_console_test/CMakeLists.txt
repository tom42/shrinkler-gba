# SPDX-FileCopyrightText: 2026 Thomas Mathys
# SPDX-License-Identifier: MIT

# TODO: implement test:
#       * compress lostmarbles.elf => OK, but where does the output go?
#         * => Ensure it goes into the build directory, somehow
#       * This should yield some output file
#       * Compare the output file

function(make_test input_file)
  add_test(
    NAME "Compress ${input_file}"
    COMMAND shrinkler-gba ${CMAKE_CURRENT_SOURCE_DIR}/lostmarbles.elf)
endfunction()

make_test(lostmarbles.elf)

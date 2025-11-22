// SPDX-FileCopyrightText: 2021 Thomas Mathys
// SPDX-License-Identifier: MIT

module shrinkler_gba;

namespace shrinkler_gba
{

void gba_packer::pack(const options&)
{
    // TODO: implement the existing 5 steps:
    //       * Load program (load ELF and convert to raw binary)
    //       * Compress raw binary
    //       * Assemble cart
    //       * Fix up for EZFlash (why is that not done by the assembler? => because that's post processing and needs to be done for both assemblers?)
    //       * Write result to disk
    // TODO: here we would like to rework the existing components, so that they do NOT produce console output. All console output should be controlled/done up here in this file
}

}

<!--
SPDX-FileCopyrightText: 2021 Thomas Mathys
SPDX-License-Identifier: MIT
shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
-->

# shrinkler-gba
shrinkler-gba is a port of the Shrinkler Amiga executable cruncher for the Gameboy Advance.

# License
shrinkler-gba is licensed under the MIT license.

The actual compression code comes from [Shrinkler](https://github.com/askeksa/Shrinkler),
which has its own [license](LICENSES/LicenseRef-Shrinkler.txt).

# Basic usage
To create a 4k or whatever intro for the Gameboy Advance using shrinkler-gba, take the following steps:
* Create your intro using your favorite programming language. Assembly, C, C++, Rust, whatever:
  * The uncompressed intro which is fed to shrinkler-gba to produce the final ROM must be an ELF file.
  * Your intro's load address should be in IWRAM or EWRAM and, obviously, so should the entry point.
  * Your intro should not contain a cartridge header. It will be added by shrinkler-gba.
    The ROM produced by shrinkler-gba has the following layout:
    * Cartridge header, with parts of the depacker code inside the header
    * Rest of depacker code, most of which is Thumb code
    * Compressed intro
  * Your intro's entry point may expect the CPU to be in ARM or Thumb state. shrinkler-gba will try to
    detect the expected state from the ELF file's `e_entry` field. If it contains an even address,
    the depacker will switch to ARM state before executing the uncompressed intro. If it is odd,
    the depacker will remain in Thumb state. **Warning**: while this works for me it is reportedly not 100%
    reliable, see e.g. [here](https://community.arm.com/support-forums/f/architectures-and-processors-forum/7513/elf-entry-point-in-thumb-mode-armv7-aarch32)
    or various threads on Stackoverflow. To be on the safe side, make your intro's entry point
    expect ARM state and switch to Thumb state yourself if needed. This will cost you 8 bytes, though.
* Compress your intro using shrinkler-gba by running the following command: `shrinkler-gba intro.elf -p9`
  * The ROM will be written to `intro.gba`. Use the `-o` option to specify a different output file name.
  * The `-p9` option will crank up compression to the highest level currently supported.
    Use a lower value or omit the option altogether to get faster compression during development.
  * To get a list of all available options, run the following command: `shrinkler-gba -?`

**A few more warnings are in order now**:
* Linker scripts and startup code supplied with toolchains such as devkitPro will not satisfy the
  requirements listed above for your intro. You'll have to roll your own.
* Since shrinkler-gba sticks part of the depacker code into the cartridge header you should not modify
  the final ROM with tools such as devkitPro's `gbafix`. Doing so is guaranteed to break the depacker code.
* The depacker does currently not preserve CPU register contents, nor does it zero memory it used during decompression.
  Your intro should therefore not rely on initial CPU register values, and it should not expect memory
  to be zero but rather clear memory itself. If this is done using the CpuSet or CpuFastSet BIOS calls,
  doing so will not eat up much space. Moreover, the code will hopefully benefit from compression since
  it is done inside the intro instead of the depacker.

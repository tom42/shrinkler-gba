<!--
SPDX-FileCopyrightText: 2021 Thomas Mathys
SPDX-License-Identifier: MIT
-->

# Things to do
## General
* Test what happens if input file is
  * A drive letter
  * A directory
  * Empty
* Have a command line option that allows selection of a particular packer
  * The option is there, but it is not yet honored
* Wrap initial version up => go 1.0
  * Test on real hardware:
    * Shrinkler depacker
    * LZSS depacker
  * Should we mention in the docs that the LZSS depacker will clobber EWRAM as tmp buffer?
* What if LZSS without H4 is smaller? Will we notice? Should we have a warning for this case?
  * We can do so, but in principle the depacker code needs to be taken into account too
  * Well if we had agbpacker we could also use that one to check manually.
* See whether/how to use clang-tidy

## Examples / Documentation
* Examples should be self-contained, no big external libraries
  * 100% assembly tiny example, for which LZSS is selected
  * C/C++ example
* Documentation: RAM Usage of depackers:
  * Shrinkler: uses top of IWRAM (give approximate size)
  * LZSS: uses end of EWRAM as temporary buffer for LZSS compressed data

## Compression and decompression
* Experiment with Shrinkler's option to disable the parity context
* Leave the GBA in a cleaner state after decompression, with the option to omit cleanup in order to save space:
  * Preserve CPU registers (not that easy for high registers in Thumb state)
  * Clear memory used by depacker (IWRAM / EWRAM)
  * Preserve sp: already implemented, but could add option to omit this for the really desperate
    * Debug stack check needs to be aware of this
    * If we save/restore other registers, can we merge save/restore of sp into general register saving code?
* Possible depacker optimizations
  * Loading of load/entry point adress: e.g. if they can be constructed with lsl/mov. Currently we always load
    them from a constant pool. At least when they're the same lzasm will already optimize this and will
    only generate one literal.

## Input file loading
* Support also loading of raw binaries, not only ELF: can do that once we need it. Anyway it will complicate things,
  since for instance it leaves us with no way of detecting whether the entry point expects ARM or Thumb state.
* Sanity checks of load address and entry point: not needed for operation, but nice to have:
  * Note: these checks do *not* belong into input_file, which should remain target independent
  * Load address must be either in IWRAM or EWRAM (not necessarily at the beginning of either area)
    * Load address + size of loaded data must not go past end of memory (IWRAM or EWRAM)
      * Max. size in IWRAM is 32K minus memory used by BIOS, and minus stack used by Shrinkler depacker, if loaded at beginning of IWRAM
      * Max. size in EWRAM is 256K, if loaded at beginning of EWRAM
      * Size of loaded data is obviously not the entire truth: in principle we'd also have to check bss section(s). Not sure whether that's worth the effort. After all it's well possible to even have bss sections in both EWRAM and IWRAM
    * Entry point must be inside memory area occupied by the binary's loaded data
  * LZSS packer:
    * The temporary buffer for the LZSS compressed data must fit into EWRAM
    * The temporary buffer for the LZSS compressed data must not overlap with the load area of the intro (unlikely, but could happen at least in theory)
  * If a packer specific sanity check fails the entire program should not abort, it's just that a particular packer is not able to compress the program and should therefore not be considered

## Old stuff below, needs clean up
* Notes from various old sources:
  * Old Thumb data depacker (shrinkler_decompress_thumb.s)
    @ * We should make sure we do not write anywhere past the context array
    @   * Well we could have a debug mode where we check this, no? Some sort of assertion?
    @ * Since the ultimate goal is to produce a GBA 4k cruncher, do we need to
    @   * Can we save some bits if we do not have our data on stack but in EWRAM?
  * The old Thumb prototype:
    @ * For starters, keep the contexts on the stack.
    @   Later we can always investigate whether having them e.g. in EWRAM is smaller.
    @   But this would require an additional register. Which we don't have.
    @ * For starters, port the thumb code as is with a single bitctx.
    @   Later we can try whether we can get smaller code if we again have
    @   bitctxl and bitctxh, but we'd have to use a high register for bitctxh.
    @ * Review getbit
    @   * Are we pushing too many registers?
    @   * Can we simplify it somehow, particularly since the endianness swap has gone?
    @ * Check whether the context table is really where we believe it is and we are
    @   not overwriting anything. Currently I am not sure this is the case.
    @   We can e.g. do so by writing something into the single bit context and then inspecting memory.
* For deploying windows binaries, do we need to take care to distribute runtime libraries? Or do we link statically?
* Open issues
  * Final binary generation/depacker
    * Depacker code
      * We never really validated it, particularly not the stack data mess
        * When we execute code, is the stack pointer pointing at the right place?
        * Did we have a buffer overrun on the stack? (In the contexts array)
* Redo our own code:
  * Do we provide the parity context option?
    * Well maybe not at first. It would mean we have to update the decompressor
    * Well, Amiga Shrinkler supports this for data only, not code. We can start like that too, if we see that an intro packs better with that option
      we can always provide it for code and assemble an appropriate depacker for that.

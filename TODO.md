<!--
SPDX-FileCopyrightText: 2021 Thomas Mathys
SPDX-License-Identifier: MIT
shrinkler-gba: Port of the Shrinkler Amiga executable cruncher for the GBA
-->

# Things to do
## Depackers
* For really small intros: use BIOS LZSS compression + Huffman encoding (4 or 8 bit, whichever is better)
  * LZSS:
    * Start with simple lazy evaluation (e.g. 1 byte lookahead)
    * Eventually should try optimal parsing
* Automatically figure out which compression scheme to use (Shrinkler or LZSS)
* Leave the system in a cleaner state, with the option to omit cleanup in order to save space:
  * Preserve CPU registers (not that easy for high registers in Thumb state)
  * Clear memory used by depacker (IWRAM / EWRAM)
  * Preserve sp: already implemented, but could add option to omit this for the really desperate
    * Debug stack check needs to be aware of this
    * If we save/restore other registers, can we merge save/restore of sp into general register saving code?

## Input file loading
* Support also loading of raw binaries, not only ELF: can do that once we need it
* Sanity checks of load address and entry point: not needed for operation, but nice to have:
  * Note: these checks do *not* belong into input_file, which should remain target independent
  * Load address must be either in IWRAM or EWRAM (not necessarily at the beginning of either area)
    * Load address + size of loaded data must not go past end of memory (IWRAM or EWRAM)
      * Max. size in IWRAM is 32K minus memory used by BIOS, and minus stack used by Shrinkler depacker, if loaded at beginning of IWRAM
      * Max. size in EWRAM is 256K, if loaded at beginning of EWRAM
      * Size of loaded data is obviously not the entire truth: in principle we'd also have to check bss section(s). Not sure whether that's worth the effort. After all it's well possible to even have bss sections in both EWRAM and IWRAM
    * Entry point must be inside memory area occupied by the binary's loaded data

## Old stuff below, needs clean up
* Next steps
  * Check decompressed data using something simple, e.g. adler32
    * Write a *simple* adler32 implementation (simple. does not need to be fast)
      * Test it with a couple of test vectors
      * Maybe also test it using lostmarbles.bin
      * Here is how to get the adler32 checksum:
        * python -c "import zlib; print(zlib.adler32(\"${file}\"))"
    * For this it would be useful if debug_check_decompressed_data_size did not destroy outp, no?
      * Well, we already know the decompressed size is good, so we can hardcode it for the CRC check.
  * Stick code in header (probably use old approach, that is, the init code). RETEST! (Orly? Do we want to take that risk?)
    * Well, I have to recheck, but getkind+getbit is 34 Thumb instructions, or 68 bytes, which is a multiple of 4 bytes.
      So we could stick getkind into the header, followed by getbit, and end up with the entry aligned.
    * If we then have our entry in the middle of the depacker code, then we should not blindly align(2).
      Instead, omit the align(2), but at runtime check and throw if current_lc() is not a multiple of 4 at the entry.
  * Wrap initial version up
* BEFORE WE DO ANY ADDITIONAL WORK ON THE DECODER
  * TURN IT AROUND ONCE, SO THAT E.G: getnumber is in the header rather than the beginning of the main loop
* Notes from various old sources:
  * Old Thumb data depacker (shrinkler_decompress_thumb.s)
    @ * Eventually we should definitely also compare original and decompressed data
    @   * And the number of bytes decompressed
    @   * And we should make sure we do not write anywhere past the conext array
    @     Well we could have a debug mode where we check this, no? Some sort of assertion?
    @ * Since the ultimate goal is to produce a GBA 4k cruncher, do we need to
    @   * Can we save some bits if we do not have our data on stack but in EWRAM?
  * Old ARM data depacker (shrinkler_decompress_arm.s)
    @ * Document the odd addressing of the contexts? In principle we can copypaste Blueberrys explanations.
    @   - Yes we want the table with the context groups
    @   - And the explanation of the addressing in getbit
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
* Somewhere, have some documentation of the depacker, including blueberrys explanation what he does with all the contexts (and the odd stack hack)
* For deploying windows binaries, do we need to take care to distribute runtime libraries? Or do we link statically?
* NEXT:
  * Verification
    * Either when reading the ELF or writing the raw binary, should we check that sh_addralign/sh_addr match?
* Open issues
  * Final binary generation/depacker
    * Depacker code
      * We never really validated it, particularly not the stack data mess
        * When we execute code, is the stack pointer pointing at the right place?
        * Did we have a buffer overrun on the stack? (In the contexts array)
* See which output in shrinkler.cpp should be optional and which should not:
  * The message about the -r option for instance should NOT be hidden if verbose mode is not used
* Ensure all source files are using spaces, not tabs
* Set up libshrinkler
  * Do we add that new bytes parameter thing?
* Redo our own code:
  * Do we provide the parity context option?
    * Well maybe not at first. It would mean we have to update the decompressor
    * Well, Amiga Shrinkler supports this for data only, not code. We can start like that too, if we see that an intro packs better with that option
      we can always provide it for code and assemble an appropriate depacker for that.
  * Are we going to have a data option? (pack/write raw data, little endian for starters, maybe later with a big endian option? [does anybody need this?])
* See whether upgrading Shrinkler is worthwile.
  * I am not even sure, but I think we are not yet at 4.7. Possibly 4.6, or something in between 4.6 and 4.7.
  * If we do upgrade, ensure we also upgrade the packer if required
  * And also if we do upgrade: apparently the Shrinkler license has slightly changed, so we'd need to update that too
  * Use preset 3 as default, as Blueberry does these days
    * Can we unhardcode this from the command line parser, if not already done so? Well, not easily, with default/min/max values all over the place. Time to write an argp c++ wrapper, really.

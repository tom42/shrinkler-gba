* NEXT: NOW WITH THE CODE IN HEADER, A test on real hardware is in order!
  * Questions:
    * Is it really legal to have a nonzero device type? (value at header offset 0xb4)
    * Is it really required that the reserved area at the end of the header (at 0xbe) is zero?
* BEFORE WE DO ANY ADDITIONAL WORK ON THE DECODER
  * TURN IT AROUND ONCE, SO THAT E.G: getnumber is in the header rather than the beginning of the main loop
* Implement relevant GBA BIOS compressors
  * Then do encoders
    * Huffman
      * Do we need to do any alignment/padding here?
        * For instance inside the data, for the raw data stream? Is this guaranteed to always be 32 bit aligned automatically?
    * LZSS
      * Do we need to do any alignment/padding here?
      * Is the GBA BIOS able to decode references into the lookahead buffer?
        * Well that would make it necessary to treat the lookahead buffer as part of the sliding window.
        * Should investigate it, since it allows for some additional improvements
        * See data compression book page 218/219
    * Eventually do optimal LZSS
* Notes from various old sources:
  * Old Thumb data depacker (shrinkler_decompress_thumb.s)
    @ * Might actually want to try the following: write a simpler (than depacker.cpp) one file depacker,
    @   and see what gcc -Os comes up with?
    @ * Eventually we should definitely also compare original and decompressed data
    @   * And the number of bytes decompressed
    @   * And we should make sure we do not write anywhere past the conext array
    @     Well we could have a debug mode where we check this, no? Some sort of assertion?
    @ * Test: does this code also work when the output buffer is at an odd address?
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
* Eventually, get rid of the fmt library (yeah, right)
* NEXT:
  * Verification
    * Either when reading the ELF or writing the raw binary, should we check that sh_addralign/sh_addr match?
* Open issues
  * Final binary generation/depacker
    * Depacker code
      * Register state: do we care? Do we preserve registers as the BIOS leaves them? Or do we have this as an option?
      * We never really validated it, particularly not the stack data mess
        * When we execute code, is the stack pointer pointing at the right place?
        * Did we have a buffer overrun on the stack? (In the contexts array)
* See which output in shrinkler.cpp should be optional and which should not:
  * The message about the -r option for instance should NOT be hidden if verbose mode is not used
* Licensing of shrinkler.cpp:
  * Since this file is mostly copypasted Shrinkler code it ought to be using the Shrinkler license, no?
* Ensure all source files are using spaces, not tabs
* Set up libshrinkler
  * README.md in libshrinkler, explaining what it is
  * Do we add that new bytes parameter thing?
* Add README.md (or 3rdparty.md)
  * Also document how to create subtrees, since I forget how to do this:
    * git subtree add --prefix 3rdparty/Shrinkler https://github.com/askeksa/Shrinkler master --squash
      * Important: do not have ./ in front of 3rdparty, Windows does not like this!
    * git subtree add --prefix 3rdparty/argp-standalone https://github.com/tom42/argp-standalone master --squash
    * git subtree add --prefix 3rdparty/ELFIO https://github.com/serge1/ELFIO Release_3.9 --squash
    * git subtree add --prefix 3rdparty/fmt https://github.com/fmtlib/fmt 8.0.1 --squash
    * git subtree add --prefix 3rdparty/lzasm https://github.com/tom42/lzasm v0.1.0 --squash
  * And also document how to update, e.g:
    * git subtree pull --prefix 3rdparty/lzasm https://github.com/tom42/lzasm v0.2.0 --squash
* Be clear about licensing (top level README.md)
  * shrinkler-gba code: MIT license
  * libshrinkler: MIT, except actual shrinkler code, which is Blueberry's
  * 3rd party code: has their own license (Shrinkler in particular)
* Redo our own code:
  * Do we provide the parity context option?
    * Well maybe not at first. It would mean we have to update the decompressor
    * Well, Amiga Shrinkler supports this for data only, not code. We can start like that too, if we see that an intro packs better with that option
      we can always provide it for code and assemble an appropriate depacker for that.
  * Are we going to have a data option? (pack/write raw data, little endian for starters, maybe later with a big endian option? [does anybody need this?])
* In lzasm, eventually revert copyright notices to what we're using for shrinkler now

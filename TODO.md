* NEXT:
  * NEED TO REWRITE INPUT FILE => rename the old implementation while doing so, and create a new input_file / input_file_test
    * When done: go through entire input_file.cpp/hpp, removing methods that are not used anymore
    * objcopy does processing section wise, not segment wise.
    * Need to do that too:
      * Iterate through the section headers
        * Determine whether to output a section header. The ELF documentation is clear on what to dump:
          * For instance: sh_addr: Address where the first byte resides if the section appears in the memory image of a process;
                          a value of 0 indicates the section does not appear in the memory image of a process.
          * So we can ignore any value with sh_addr=0, which filters out most sections
          * We're then hopefully left with PROGBITS and NOBITS sections
            * NOBITS sections do not go into the output file, even if their size is nonzero
            * PROGBITS sections go into the output file, but they may have a size of zero in which case it appears nothing is written to the binary file
            * No that's not working: there may well be other section header types.
              * So perhaps just special handle NOBITS and copy everything else. I suppose that's what objcopy does too
          * ANYWAY, THAT IS JUST QUICK NOTES. READ THE ELF DOCUMENTATION WHEN ACTUALLY IMPLEMENTING THUS
      * Sections do not appear to have an order
        * So we must prepare a list of sections to dump first
        * Then we can sort them by address (note: sections cannot overlap)
        * Finally the logic is the same:
          * Start writing the first section. Initially, no padding is needed, set output address
            * Copy section data to file, increment output address
          * Next section:
            * Pad with zeroes and increment output address until output address of next section is reached
            * Copy section data, increment output address
      * That should pass tests for lostmarbles.elf and thumb_entry.elf
      * Any maybe we have a more complex example written with devKitPro?
  * Port input_file:
    * Here at least add ARM/Thumb entry point detection
    * This will require a note because we must adapt the depacker code because it does not support that scenario yet
      * Should probably start writing real test binaries for the GBA then, so that we can actually test stuff
    * Tests:
      * Test ARM/Thumb entry point detection
      * File with entry different from load address (so we can see we have not messed anything up)
      * ...
  * Archive open issues from
    * Old gbaic
* Open issues
  * Final binary generation/depacker
    * Should be updated, so that it can execute ARM and Thumb entry points. Detection is done by input_file
      * Do we check whether the entry point is valid?
      * Do we attempt to optimize loading of the entry point? (ldr= vs. some move/shift/rotate combo)?
      * Attention: load address != entry point. The case where they're the same is a special case!
        * The binary must be decompressed to the load address
        * Execution must start at the entry point
    * GBA HEADER
      * The checksum must be calculated!
      * Do we stick code into it? If so, what code? Ideally it is code that does not normally need to be modified!
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
  * Somehow a translation unit that sucks in all needed shrinkler source
  * A unit test: do we have something like this? (just need a single one we can use to get things up and running)
* Add README.md (or 3rdparty.md)
  * Document what's inside this directory
    * Also add link to our main source of documentation: https://www.atlassian.com/git/tutorials/git-subtree
  * Also document how to create subtrees, since I forget how to do this:
    * git subtree add --prefix 3rdparty/Shrinkler https://github.com/askeksa/Shrinkler master --squash
      * Important: do not have ./ in front of 3rdparty, Windows does not like this!
    * git subtree add --prefix 3rdparty/argp-standalone https://github.com/tom42/argp-standalone master --squash
    * git subtree add --prefix 3rdparty/ELFIO https://github.com/serge1/ELFIO Release_3.9 --squash
    * git subtree add --prefix 3rdparty/fmt https://github.com/fmtlib/fmt 8.0.1 --squash
    * git subtree add --prefix 3rdparty/lzasm https://github.com/tom42/lzasm v0.1.0 --squash
* Be clear about licensing (top level README.md)
  * shrinkler-gba code: MIT license
  * libshrinkler: MIT, except actual shrinkler code, which is Blueberry's
  * 3rd party code: has their own license (Shrinkler in particular)
* Redo our own code:
  * Do we provide the parity context option?
    * Well maybe not at first. It would mean we have to update the decompressor
* In lzasm, eventually revert copyright notices to what we're using for shrinkler now

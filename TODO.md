* classes
  * Declare classes as final?
* Do not forget to have a look at README.md from shrinkler-arm
  * And also at the depacker source from shrinkler-arm
  * For instance, the depacker is not yet preserving registers!
  * And, on the gba, if we generate code, do we need to flush a cache of some sort or does jumping to the code suffice since that invalidates the prefetch queue?
* For deploying windows binaries, do we need to take care to distribute runtime libraries? Or do we link statically?
* Eventually, get rid of the fmt library (yeah, right)
* NEXT:
  * https://stackoverflow.com/questions/57103912/how-does-objcopy-compute-what-sections-for-the-elf-file-to-insert-into-the-outpu
    * Maybe write a semi-helpful answer here?
      * Naive implementation: just use PT_LOAD segments
      * However, that's not what objcopy appears to do
        * My sample ELF file
        * Objcopy documentation, where it says that the dump starts with the lowest section.
          * I believe they really mean section and not segment.
      * Some people claim looking at the alloc flag is enough, but I think it is not,
        since NOBIT sections can have alloc set too. But they are not part of the raw binary image.
  * Verification
    * Either when reading the ELF or writing the raw binary, should we check that sh_addralign/sh_addr match?
  * Port input_file:
    * This will require a note because we must adapt the depacker code because it does not support that scenario yet
      * Should probably start writing real test binaries for the GBA then, so that we can actually test stuff
    * Tests:
      * File with entry different from load address (so we can see we have not messed anything up)
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

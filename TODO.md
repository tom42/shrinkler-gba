* main program:
  * Set up option parsing
    * This requires the option class
    * And then some parse_command_line function
* Set up libshrinkler-gba
  * First step would be command line parsing
    * That would then also lead to argp probing
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
* Set up 3rd party dependencies:
  * In any case, might want to have a README.md in there? or 3rdparty.md?
  * Merge dependencies into subtree (there is nothing to do for ELFIO, which is header only, and for shrinkler, for which we'll create a special library)
    * fmt: compile it, but only if it is not found otherwise
    * elfio: compile it, but only if it is not found otherwise
* Be clear about licensing (top level README.md)
  * shrinkler-gba code: MIT license
  * libshrinkler: MIT, except actual shrinkler code, which is Blueberry's
  * 3rd party code: has their own license (Shrinkler in particular)
* Redo our own code:
  * Do we provide the parity context option?
    * Well maybe not at first. It would mean we have to update the decompressor
* In lzasm, eventually revert copyright notices to what we're using for shrinkler now

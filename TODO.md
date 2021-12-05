* Set ub libshrinkler
  * README.md in libshrinkler, explaining what it is
  * A single header
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
* Set up 3rd party dependencies:
  * In any case, might want to have a README.md in there? or 3rdparty.md?
  * Merge dependencies into subtree (there is nothing to do for ELFIO, which is header only, and for shrinkler, for which we'll create a special library)
    * argp: compile it, but only if native argp is not present
    * fmt: compile it, but only if it is not found otherwise
    * elfio: compile it, but only if it is not found otherwise
* Be clear about licensing (top level README.md)
  * shrinkler-gba code: MIT license
  * libshrinkler: MIT, except actual shrinkler code, which is Blueberry's
  * 3rd party code: has their own license (Shrinkler in particular)

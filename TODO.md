* Add README.md (or 3rdparty.md)
  * Document what's inside this directory
  * Also document how to create subtrees, since I forget how to do this:
    * git subtree add --prefix 3rdparty/Shrinkler https://github.com/askeksa/Shrinkler master --squash
      * Important: do not have ./ in front of 3rdparty, Windows does not like this!
    * git subtree add --prefix 3rdparty/argp-standalone https://github.com/tom42/argp-standalone master --squash
* Set up 3rd party dependencies:
  * 3rdparty/subtree vs. just 3rdparty
  * In any case, might want to have a README.md in there? or 3rdparty.md?
  * Obtain dependencies again, all as subtree
    * elfio (3.9 or whatever is latest)
    * fmt (do we really need this? get rid of it if easily possible)
* Be clear about licensing (top level README.md)
  * shrinkler-gba code: MIT license
  * 3rd party code: has their own license (Shrinkler in particular)

* Need to redo the release of lzasm:
  * Fix reference to gbaic, is now shrinkler-gba
  * Either delete release 0.1.0, or create a release 0.1.1. Whatever works.
* Set up 3rd party dependencies:
  * 3rdparty/subtree vs. just 3rdparty
  * In any case, might want to have a README.md in there? or 3rdparty.md?
  * Obtain dependencies again, all as subtree
    * argp-standalone
    * latest shrinkler
    * elfio (3.9 or whatever is latest)
    * fmt (do we really need this? get rid of it if easily possible)
* Be clear about licensing (top level README.md)
  * shrinkler-gba code: MIT license
  * 3rd party code: has their own license (Shrinkler in particular)

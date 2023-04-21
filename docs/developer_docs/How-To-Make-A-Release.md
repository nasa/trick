| [Home](/trick) → [Developer Docs](Developer-Docs-Home) → How to make a release |
|------------------------------------------------------------------|

# How To Make A Trick Release
This guide describes how to make a Trick release on GitHub
### Close Open Issues
Go through recently updated issues/pull requests and make sure that any open issues that have been fixed are merged and closed
### Make the release notes
#### Examples
https://github.com/nasa/trick/releases
#### Steps
* Review closed issues, pull requests, and commits since the last release and make a bulleted list of major changes
  * UI Changes
  * Header Changes
  * Dependency Changes
  * Major Bugfixes
  * New features, tools, and example sims
  * New OS/distro support
  * New dependency support (such as LLVM/GCC versions)
  * Major Documentation changes
### Choose a version number
* Advance major version number if there are interface changes or other major build-breaking changes
* Advance minor version number if there are major improvements or new features that are not build breaking
* Advance patch/tiny version number for hotfixes or other bugfixes
### Change the version numbers in the master branch
#### Example
https://github.com/nasa/trick/commit/a317c222748e706e89b7f344f6910d1f39824feb
#### Steps
* In share/trick/trick_ver.txt change the "current_version" string to match the new version number and remove any suffixes (like: "-beta").
* In trick_source/java/pom.xml change the \<version\> tag to the new version and remove any suffixes.
* In CMakeLists.txt change TRICK_MAJOR TRICK_MINOR TRICK_TINY to match the current version, and set TRICK_PRERELEASE to an empty string
### Create a commit, tag, and push to github.com
  * `git commit -m "update version numbers for Trick Simulation Environment X.Y.Z"`
  * `git tag -a <Version Number> -m "Trick Simulation Environment <Version Number>"` e.g. `git tag -a 25.0.0 -m "Trick Simulation Environment 25.0.0"`
  * `git push origin <tagname>`
### Change version numbers back to prerelease and push
  * Reverse the process of changing the version numbers in the files listed above; update to the next minor prerelease version (or major version if planning a major release soon). 
  * Remember to add the -beta suffix to the version number
  * `git commit -m "update version numbers to prerelease X.Y.Z-beta"`
  * `git push origin master`
### Create the release through the github UI
  * This process is subject to change, so I'm posting the link to GitHub documentation here:
    * https://docs.github.com/en/repositories/releasing-projects-on-github/managing-releases-in-a-repository#creating-a-release
  * Currently we do not add any artifacts/binaries or open discussions, so you can ignore those optional steps. 

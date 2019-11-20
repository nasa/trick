### Simulation Compilation Environment Variables

The -Ipaths in TRICK_CFLAGS and TRICK_CXXFLAGS tell Trick where to find model source files.  The flags also can contain compiler settings, for instance the -g flag is used for compiling in debug mode. See section Trick_Environment for more detail and information on variables for linking in external libraries, setting the compiler etc.

### Making the Simulation for the First Time.

Makefiles contain all of the rules for building the simulation. When a simulation is ready to be built for the very first time, the configuration processor script (CP) is executed in the simulation directory.

```
UNIX prompt> trick-CP
```

CP creates a small makefile and calls "make" to start the simulation build process. The small makefile is the same from simulation to simulation. It can even be copied from another simulation directory, skipping the CP step entirely. Once the small makefile is created, CP does not need to be run again, compilation can be accomplished by running "make".

When make is invoked, it calls CP to process the user-created S_define file. CP finds all source code related to the simulation, builds the code using a C/C++ compiler, and puts it all together to make one executable. CP also builds code generated from user source.  This is done for model documentation, Python input, unit specifications, variable descriptions etc.

After the initial CP is run, when there are changes made to model source code or the S_define file, they are recompiled using make.

```
UNIX prompt> make
```

### How Trick Finds Simulation Source Code

Trick compiles all user model source code referenced in the S_define either through file inclusion or user supplied "library dependencies".  Trick begins at the S_define and recursively follows code dependencies to create the entire source tree.

Header files are double pound "##" included in the S_define.  Each header file is recursively parsed to determine all lower level header files on which the top level header is dependent on. Doing this for all double pound files yields the full list of header files.

Model source files are found through LIBRARY DEPENDENCIES specified in the Trick header comment at the top of the S_define and model source files.  A recursive search is made beginning at the S_define, following all model source and header files. For details, see S_define_Library_Dependencies and Library_Dependencies.

Once the entire source tree is created, rules to compile all of these files are written to the makefile.

### Changing Simulation Compilation through Makefile Overrides

Sometimes a programmer may want Trick to pick up specific compiler flags or some special makefile rule for building a model or building the simulation. Trick allows the programmer to override the default Makefile rules with a facility we are calling "makefile overrides".

For overrides in model directories, a user can create a file called `makefile_overrides`. This file allows the user to augment or override rules in the main simulation `makefile`. `makefile_overrides` applies to all files in the directory in which it is located. Additionally, special treatment is afforded to directories specifically named `src`. In such directories, if no `makefile_overrides` exists, the parent directory is searched as well, and any `makefile_overrides` found there applies to the files in `src` in addition to the parent directory. Again, searching of the parent directory only occurs for directories named `src`, and the search does not extend beyond the immediate parent directory. This special treatment of `src` is a relic of the past, and you should prefer putting `makefile_overrides` in the same directory as the source files themselves.

Additional make-like syntax is available in `makefile_overrides` to apply changes to subsets of files. `objects:` will be replaced with all object files to be created from all source files in the directory. For instance,

```make
objects: TRICK_CXXFLAGS += -Wall
```

will cause all source files in the directory to be compiled with the additional flags. You can select a subset of files by extension by prepending `objects:` with the extension to affect:

```make
cpp_objects: TRICK_CXXFLAGS += -Wextra
```
will cause only files ending in `.cpp` to receive the additional flags.

Other possibilities are: ```c_objects``` , ```f_objects``` , ```l_objects``` , ```y_objects``` , ```C_objects``` , ```cc_objects``` , ```cxx_objects``` , and
```CPLUSPLUS_objects``` .

For overrides in sim directories, there is a sim specific overrides file called `S_overrides.mk`. If this file is present in the sim directory, it is included after the directory-specific overrides. The rules in this file are the last word in how things are going to compile.

#### Example Of How To Add a Pre-compiled Library to the Simulation

Go to simulation dir.

```
UNIX Prompt> cd /user/me/trick_sims/SIM_ball_L1
```

Edit a file called "S_overrides.mk". Append to the TRICK_USER_LINK_LIBS variable.

```
TRICK_USER_LINK_LIBS = -L/path/to/library -lmy_lib
```
#### Example Of How To Exclude a Directory from ICG during CP

Go to simulation dir.

```
UNIX Prompt> cd /user/me/trick_sims/SIM_ball_L1
```

Edit a file called "S_overrides.mk". Append to the TRICK_ICG_EXCLUDE variable.

TRICK_ICG_EXCLUDE += /path/to/exclude:/another/path/to/exclude

#### Example Of How To Exclude a Directory from most CP processing

Edit a file called "S_overrides.mk". Append to the TRICK_EXCLUDE variable.

```
TRICK_EXCLUDE += /path/to/exclude:/another/path/to/exclude
```

### Cleaning Up

There are several levels of clean.

```
UNIX Prompt> make clean
```

Clean tries to remove only object files directly related to the current simulation. It will remove all of the generated files in the simulation directory. Clean also selectively removes model object files used to link this simulation.

```
UNIX Prompt> make spotless
```

Spotless is less discriminate in the files it removes. In addition to all of the files that clean removes, spotless will remove complete model object directories where any file included in the simulation was found.

```
UNIX Prompt> make apocalypse
```

Apocalypse is a special case rule when simulation libraries are used to build a simulation. See section Simulation_Libraries for more information about. In addition to all of files that spotless removes, apocalypse will run the spotless rule on any simulation directory the current simulation includes.

[Continue to Trickified Project Libraries](Trickified-Project-Libraries)

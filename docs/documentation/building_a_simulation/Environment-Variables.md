Trick uses a list of variables for building sims e.g. TRICK_CFLAGS and TRICK_CXXFLAGS.  Each variable has a default value that may be overridden by setting the value in the environment. Trick resolves these variables by a call to a function called "trick-gte". Type in "${TRICK_HOME}/bin/trick-gte" on the command line to see what the "Trick environment" is.

### Adding ${TRICK_HOME}/bin to PATH

${TRICK_HOME}/bin can be added to the PATH environment variable for convenience. It is not necessary for compiling or running sims but allows you to call Trick's functions without using the full path of Trick's executables.

```
# bash
PATH="${PATH}:/path/to/trick"

# [t]csh
setenv PATH "${PATH}:/path/to/trick"
```

### TRICK_CFLAGS and TRICK_CXXFLAGS

The contents of TRICK_CFLAGS is included on the command line with each C file compilation. Similarly, TRICK_CXXFLAGS is included for each C++ file. Each contain header file search directories, macro define variables, and compiler flags.

For building a simulation, a user must be proficient at tweaking TRICK_CFLAGS and TRICK_CXXFLAGS. There are a several ways to do this.

TRICK_CXXFLAGS works exactly like TRICK_CFLAGS.

Example 1: Add "-I/user/mooo/trick_models" to the environment variable. TRICK_CFLAGS is currently...

```
UNIX Prompt> echo $TRICK_CFLAGS
-Wall
```

Now we need to edit the shell startup file where TRICK_CFLAGS is defined.

Add the following line for bash.

```
TRICK_CFLAGS="$TRICK_CFLAGS -I/user/mooo/trick_models"
```

Add the following line for [t]csh.

```
setenv TRICK_CFLAGS "$TRICK_CFLAGS -I/user/mooo/trick_models"
```

Now source your shell startup file and voila!...

For bash:
```
UNIX Prompt> . ~/.bash_profile
UNIX Prompt> echo $TRICK_CFLAGS
-Wall -I/user/mooo/trick_models
```

For [t]csh:
```
UNIX Prompt> source ~/.cshrc
UNIX Prompt> echo $TRICK_CFLAGS
-Wall -I/user/mooo/trick_models
```

Example 2: Add "-I/user/mooo/trick_models" through the simulation S_overrides.mk file

We need to edit the S_overrides.mk file in the simulation to be built.

Add the following line.

```
TRICK_CFLAGS="$TRICK_CFLAGS -I/user/mooo/trick_models"
```

This will not show up in the current shell environment, but will be set for each command that the makefile executes.

### TRICK_CONVERT_SWIG_FLAGS

TRICK_CONVERT_SWIG_FLAGS contains flags sent to the convert_swig utility. Currently the flags only support "-s" which allows convert_swig to process STLs.

### TRICK_SWIG_FLAGS and TRICK_SWIG_CFLAGS

TRICK_SWIG_FLAGS are the options that are passed to SWIG (see the SWIG documentation). TRICK_SWIG_CFLAGS are the the options passed to the c/c++ compiler when compiling SWIG objects. 

### TRICK_EXCLUDE

A colon separated list of directories to skip when processing files.

It is possible to instruct all CP functions to skip entire directories using the environment variable TRICK_EXCLUDE. Set this variable to a colon separated list of directories which you wish CP to bypass. All header files found in TRICK_EXCLUDE will not be processed. All source code files found in TRICK_EXCLUDE will not be compiled or linked into the simulation.

This feature is useful to bring in packages as a library.

### TRICK_FORCE_32BIT

To force Trick to compile in 32-bit on 64-bit systems, set the TRICK_FORCE_32BIT environment variable to 1. Setting this variable appends "-m32" automatically to TRICK_CFLAGS and TRICK_CXXFLAGS.
4.2.6 TRICK_HOST_CPU

Trick determines a system specific suffix to append to object code directory names. By default this is determined automatically by Trick. The user may override this by setting the TRICK_HOST_CPU environment variable.

### TRICK_ICG_EXCLUDE

A colon separated list of directories to skip when processing header files.

It is possible to instruct ICG to skip entire directories using the environment variable TRICK_ICG_EXCLUDE. Set this variable to a colon separated list of directories which you wish ICG to bypass. This is useful when there is code which you do not wish Trick to have any knowledge of (i.e. you don’t need any of the parameters recorded or input processable).

### TRICK_LDFLAGS

TRICK_LDFLAGS include linker flags. TRICK_LDFLAGS is used when linking the simulation executable. It is rare to set this variable.

### TRICK_SFLAGS

TRICK_SFLAGS includes header file search directories and macro define variables. TRICK_SFLAGS is used when parsing the S_define file.

### TRICK_USER_LINK_LIBS

Additional library and library directories to include when linking a simulation.

An example of adding a library search path, a library to be searched for, and a full path library.

bash
```
export TRICK_USER_LINK_LIBS="-L/full/path/to/libs -lfile1 /another/path/to/a/libfile2.a"
```

[t]csh
```
setenv TRICK_USER_LINK_LIBS "-L/full/path/to/libs -lfile1 /another/path/to/a/libfile2.a"
```
### TRICK_GTE_EXT

`TRICK_GTE_EXT` allows you to compile exported `make` variables into your sim executable so that default values are available for them at run time. These "baked in" variables will use the default compile-time values only if they are not already defined at run time. You do this by adding them to `TRICK_GTE_EXT`, which is a space-delimited list of names.

**[Defining Variables](https://www.gnu.org/software/make/manual/html_node/Environment.html)**  
You can define a variable directly in a makefile by making an assignment to it. However, variables can also come from the environment in which `make` is run. Every environment variable that `make` sees when it starts up is transformed into a `make` variable with the same name and value. However, an explicit assignment in a makefile, or with a command argument, overrides the environment.

**[Exporting Variables](https://www.gnu.org/software/make/manual/html_node/Variables_002fRecursion.html)**  
`make` exports a variable if any of the following are true:

1. it is defined in the environment initially
1. it is set on the command line
1. it is preceded by the `export` keyword in a makefile

In all cases, the name must consist only of letters, numbers, and underscores.

```make
export VAR1 = potato
export VAR2 = flapjack
VAR3 = banana

TRICK_GTE_EXT += VAR1 VAR3 VAR4

```

At run time:
* `VAR1` will default to `potato`.
* `VAR2` will be undefined by default, as it was not added to `TRICK_GTE_EXT`.
* `VAR3` will be undefined by default if it was not present in the environment at compile time, as it was not explicitly `export`ed in the makefile. If it *was* present in the environment at compile time, it will default to `banana`, as such variables are automatically exported, and explicit assignments override environment values in `make`.
* `VAR4` will default to its compile-time environment value, if any.
* For each variable, the default value will only be used if that variable is not present in the environment at run time.

### MAKEFLAGS

MAKEFLAGS is not a Trick environment variable. It is used with the GNU make utility. Invoking make with the -j flag allows make to compile more than one file simultaneously. Dramatic speedups in compiling can be achieved when using multiple processors.

```
UNIX Prompt> setenv MAKEFLAGS –j10
```

[Continue to Simulation Definition File](Simulation-Definition-File)

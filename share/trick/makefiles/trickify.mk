# This file can be used to create an object file containing the io_* and py_*
# code that Trick would normally generate during the simulation build process.
# Sims can then link against this object file, reducing compilation time.
#
# To use it, create a directory that includes a file named S_source.hh that
# includes all header files for which you want io_* and py_* code generated.
# This is not the S_source.hh that Trick generates during compilation, but
# Trick's tools assume this name for now, so you have to use it. Then, in that
# directory, run the following command:
#
# make -f $(TRICK_HOME)/share/trick/makefiles/trickify.mk
#
# You can, of course, call this from your own makefile. But remember to switch
# to the directory containing your S_source.hh first using make's -C option if
# necessary. For instance:
#
# all:
# 	$(MAKE) -C <directory> -f $(TRICK_HOME)/share/trick/makefiles/trickify.mk
#
# There are a number of variables you can export from your own makefile or
# define on the command line when calling make:
#
# TRICKIFY_CXX_FLAGS (required)
# Define this variable to specify compiler flags. Most importantly, include the
# paths to the header files included by your S_source.hh. For instance:
# -I$(HOME)/myproject/foo/include -I$(HOME)/myproject/bar/include
#
# TRICKIFY_OBJECT_NAME (optional)
# The name of the generated object file. The default value is trickified.o.
# You should choose something more meaningful, like trickified_myproject.o.
#
# TRICKIFY_PTYON_DIR (optional)
# The directory into which generated Python modules are placed. The default
# value is python (in the current directory).
#
# -----------------------------------------------------------------------------
#
# EXAMPLE:
#
# For a project with the following structure:
#
# $(HOME)/potato/
#     include/
#     trickified/
#         S_source.hh
#         Makefile
#
# The following Makefile would be appropriate:
#
# ifndef TRICK_HOME
#     $(error TRICK_HOME must be set)
# endif
#
# TRICKIFY := $(TRICK_HOME)/share/trick/makefiles/trickify.mk
#
# ifeq ($(wildcard $(TRICKIFY)),)
#     $(error This makefile requires at least Trick 17.1)
# endif
#
# export TRICKIFY_CXX_FLAGS := -I$(HOME)/potato/include
# export TRICKIFY_OBJECT_NAME := trickified_potato.o
#
# all:
# 	$(MAKE) -f $(TRICKIFY)
#
# clean:
# 	rm -rf $(TRICKIFY_OBJECT_NAME) build python
#
# -----------------------------------------------------------------------------
#
# For more information, see:
# github.com/nasa/trick/wiki/Trickified-Project-Libraries

ifndef TRICKIFY_CXX_FLAGS
    $(error TRICKIFY_CXX_FLAGS must be set)
endif

TRICKIFY_OBJECT_NAME ?= trickified.o
TRICKIFY_PYTHON_DIR ?= python
TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../../..)

ifneq ($(wildcard build),)
    OBJECTS := $(shell find build -name "*.i")
    OBJECTS := $(join $(dir $(OBJECTS)),$(patsubst %.i,%.cpp,$(notdir $(OBJECTS))))
    OBJECTS += $(shell find build -name "*.cpp")
    OBJECTS := $(OBJECTS:.cpp=.o)
endif

include $(TRICK_HOME)/share/trick/makefiles/Makefile.common
TRICK_CFLAGS   += $(TRICKIFY_CXX_FLAGS)
TRICK_CXXFLAGS += $(TRICKIFY_CXX_FLAGS)

# Ensure we can process all headers
TRICK_EXT_LIB_DIRS :=

# When given a library, the linker will only link in objects that are known to
# be needed at link time. However, Trick uses dlsym to dynamically load the
# objects we'll be creating here. It cannot be known which objects will be
# needed at link time, so the sim has to link them all. In that case, we might
# as well create an object (which will be fully linked in by the sim) instead
# of a library (which would require the use of the -whole-archive option).
#
# One disadvantage of this approach is that we have to link all of the objects
# in this rule no matter how many have changed, whereas ar would allow us to
# replace only the changed objects. However, Trickified projects are
# necessarily used as third-party libraries, and so would be expected to
# change infrequently. Moreover, the methods for force-linking a library differ
# between Linux and Mac, so obviating the need for it simplifies a Trickified
# project's user-facing makefile.
$(TRICKIFY_OBJECT_NAME): $(OBJECTS) | $(dir $(TRICKIFY_OBJECT_NAME))
	$(info $(call COLOR,Linking)    $@)
	@ld -r -o $@ $^

$(dir $(TRICKIFY_OBJECT_NAME)) $(TRICKIFY_PYTHON_DIR):
	@mkdir -p $@

%.cpp: %.i | $(TRICKIFY_PYTHON_DIR)
	$(info $(call COLOR,SWIGing)    $<)
	@$(SWIG) $(TRICK_INCLUDE) $(TRICK_DEFINES) $(TRICK_VERSIONS) $(SWIG_FLAGS) -c++ -python -includeall -ignoremissing -w201,303,325,362,389,401,451 -outdir $(TRICKIFY_PYTHON_DIR) -o $@ $<

%.o: %.cpp
	$(info $(call COLOR,Compiling)  $<)
	@$(TRICK_CC) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(PYTHON_INCLUDES) -std=c++11 -Wno-invalid-offsetof -Wno-shadow -c -o $@ $<

# $(OBJECTS) is meant to contain all of the py_* and io_* object file names. We
# can't construct those until we run ICG and convert_swig. But we can't run the
# rule for ICG and convert_swig before $(OBJECTS) is expanded because it is a
# prerequisite of $(TRICKIFY_OBJECT_NAME), and prerequisites are always
# immediately expanded. Therefore, when make processes this file (for the first
# time after a clean), $(OBJECTS) is empty, because the find will be executed
# before ICG and convert_swig have created any files. What we really want is to
# run ICG and convert_swig before $(OBJECTS) is expanded.
#
# We can do this by taking advantage of (abusing) make's behavior in the
# presence of included files. When you include another file, make attempts to
# update that file, and then re-executes the original makefile with a clean
# slate if any of the included files changed. So we need a rule that:
#
# 1. Executes only if S_source.hh or anything in its include tree changes.
# 2. Runs ICG and convert_swig to generate py_* and io_* files.
# 3. Updates its dependency file.
#
# We can then include the dependency file to trigger make to re-execute this
# main makefile if changes were detected, thus updating $(OBJECTS) to the
# latest results of ICG and convert_swig.
#
# gcc's option to automatically generate dependency information is exactly
# what we need. It will produce a file containing a make rule whose
# dependencies are everything in S_source.hh's include tree plus S_source.hh
# itself. By telling gcc to make the target of the rule the same target we use
# to execute the gcc call, we end up with a target that maintains its own
# dependency list. The method is laid out in more detail here:
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

# CURDIR is used to produce absolute paths in S_source.d, which allows it to be
# included from external makefiles. It is specifically intended to be included
# from a Trickified project's user-facing makefile. That is, the one the user
# is intended to include in a sim's S_overrides.mk. This allows the project to
# automatically build their Trickified object as part of a simulation build,
# but only when necessary as specified by the dependencies.

$(CURDIR)/build/S_source.d:
	@$(TRICK_HOME)/bin/trick-ICG $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(TRICK_ICGFLAGS) S_source.hh
	@$(TRICK_HOME)/libexec/trick/make_makefile_swig
	@$(TRICK_HOME)/libexec/trick/convert_swig
	@$(TRICK_CC) -MM -MP -MT $@ -MF $@ $(TRICKIFY_CXX_FLAGS) $(CURDIR)/S_source.hh

-include $(CURDIR)/build/S_source.d

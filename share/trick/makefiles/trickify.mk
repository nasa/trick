# This file can be used to create a library containing the io_* and py_* code that Trick would
# normally generate during the simulation build process. Sims can then link against this library,
# reducing compilation time.
#
# To use it, create a directory that includes a file named S_source.hh that includes all header
# files for which you want io_* and py_* code generated. This is not the S_source.hh that Trick
# generates during compilation, but Trick's tools assume this name for now, so you have to use it.
# Then, in that directory, run the following command:
#
# make -f $(TRICK_HOME)/share/trick/makefiles/trickify.mk
#
# You can, of course, call this from your own makefile. But remember to switch to the directory
# containing your S_source.hh first using make's -C option if necessary. For instance:
#
# all:
# 	$(MAKE) -C <directory> -f $(TRICK_HOME)/share/trick/makefiles/trickify.mk
#
# There are a number of variables you can export from your own makefile or define on the
# command line when calling make:
#
# TRICKIFY_CXX_FLAGS (required)
# Define this variable to specify compiler flags. Most importantly, include the paths to the header
# files included by your S_source.hh.
# For instance: -I$(HOME)/myproject/foo/include -I$(HOME)/myproject/bar/include
#
# TRICKIFY_LIB_NAME (optional)
# The name of the generated library file. The default value is libtrickified.a. You should
# choose something more meaningful, like libmyproject_trick.a.
#
# TRICKIFY_PTYON_DIR (optional)
# The directory into which generated Python modules are placed. The default value is python (in
# the current directory).
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
# export TRICKIFY_LIB_NAME  := libpotato_trick.a
#
# all:
# 	$(MAKE) -f $(TRICKIFY)
#
# clean:
# 	rm -rf $(TRICKIFY_LIB_NAME) build python
#
# -----------------------------------------------------------------------------
#
# See github.com/nasa/trick/wiki/Trickified-Project-Libraries for more information.

ifndef TRICKIFY_CXX_FLAGS
    $(error TRICKIFY_CXX_FLAGS must be set)
endif

TRICKIFY_LIB_NAME ?= libtrickified.a
TRICKIFY_PYTHON_DIR ?= python
TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../../..)

ifneq ($(wildcard build),)
    OBJECTS := $(shell find build -name "*.i")
    OBJECTS := $(join $(dir $(OBJECTS)),$(patsubst %.i,py_%.cpp,$(notdir $(OBJECTS))))
    OBJECTS += $(shell find build -name "*.cpp")
    OBJECTS := $(OBJECTS:.cpp=.o)
endif

include $(TRICK_HOME)/share/trick/makefiles/Makefile.common
TRICK_CFLAGS   += $(TRICKIFY_CXX_FLAGS)
TRICK_CXXFLAGS += $(TRICKIFY_CXX_FLAGS)

# Ensure we can process all headers
TRICK_EXT_LIB_DIRS :=

# While it would be nice to invoke the implicit archiving rule via a target like:
#
# $(TRICKIFY_LIB_NAME): $(TRICKIFY_LIB_NAME)($(OBJECTS))
#
# ar is not thread-safe, and, thus, we would have to disable parallelism via
# .NOTPARALLEL (a file-wide flag), significantly slowing the build process.

$(TRICKIFY_LIB_NAME): $(OBJECTS) | $(dir $(TRICKIFY_LIB_NAME))
	$(info $(call COLOR,Linking)    $@)
	@ar rsc $@ $?

# $(OBJECTS) is meant to contain all of the py_* and io_* object file names. We can't construct
# those until we run ICG and convert_swig. But we can't run the rules for ICG and convert_swig
# before $(OBJECTS) is expanded because it is a prerequiste of $(TRICKIFY_LIB_NAME), and prerequisites are always
# immediately expanded. Therefore, when make processes this file (for the first time after a clean),
# $(OBJECTS) is empty, because the find will be executed before ICG and convert_swig have created
# any files. What we really want is to run ICG and convert_swig before $(OBJECTS) is expanded.
#
# We can do this by taking advantage of (abusing) make's behavior in the presence of included
# makefiles. When you include another makefile, make attempts to update that file, and then
# reexecutes the original makefile with a clean slate if any of the included makefiles changed. So
# we need a rule that will always run ICG and convert_swig to check for header changes and update
# some file only when changes have occurred. We can then include that "marker" file to trigger make
# to reexecute this main makefile, thus updating $(OBJECTS) to the latest results of ICG and
# convert_swig.
#
# The file that fits our purpose is build/Makefile_ICG, which is only updated when convert_swig
# detects header changes. By including and making a rule for that file, we can run ICG and
# convert_swig, reexecute this makefile if necessary, and update $(OBJECTS) before running the rule
# for $(TRICKIFY_LIB_NAME). To force this rule to always execute, we add a prerequisite on a non-existent file, a
# role the icg rule can conveniently fill. Note that the normal methods of using :: or .PHONY do not
# work on rules for making makefiles, as they would cause infinite reexecution.
#
# Ultimately, it looks like this:
#  1. make starts
#  2. make includes build/Makefile_ICG
#  3. make looks for a rule to update build/Makefile_ICG
#  4. the build/Makefile_ICG rule depends on icg
#  5. icg doesn't exist, so the icg rule is run, which executes ICG
#  6. the build/Makefile_ICG rule is run, which executes convert_swig
#  7. if convert_swig detects header changes, it updates the build/Makefile_ICG file
#  8. if the build/Makefile_ICG file changed, make reexecutes the top makefile (this file)
#  9. repeat steps 1 through 6
# 10. although ICG and convert_swig are run again, there have been no header changes, so
#     build/Makefile_ICG is not updated
# 11. since no included makefiles have changed, make continues processing the top makefile (this
#     file)
# 12. $(OBJECTS) now contains an updated list of files
# 13. $(TRICKIFY_LIB_NAME) can be executed with the correct prerequisites
#
# Note that build/Makefile_ICG is included after our main target since it contains targets itself,
# and we want make without an argument to build $(TRICKIFY_LIB_NAME).
#
# Note also that this executes ICG and convert_swig every time this file is processed, regardless
# of the target. Wasteful, but acceptable.

-include build/Makefile_ICG

build/Makefile_ICG: icg
	@$(TRICK_HOME)/libexec/trick/make_makefile_swig
	@$(TRICK_HOME)/libexec/trick/convert_swig

icg:
	@$(TRICK_HOME)/bin/trick-ICG $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(TRICK_ICGFLAGS) S_source.hh

$(dir $(TRICKIFY_LIB_NAME)) $(TRICKIFY_PYTHON_DIR):
	@mkdir -p $@

py_%.cpp: %.i | $(TRICKIFY_PYTHON_DIR)
	$(info $(call COLOR,SWIGing)    $<)
	@$(SWIG) $(TRICK_INCLUDE) $(TRICK_DEFINES) $(TRICK_VERSIONS) -c++ -python -includeall -ignoremissing -w201,362,389,451 -outdir $(TRICKIFY_PYTHON_DIR) -o $@ $<

%.o: %.cpp
	$(info $(call COLOR,Compiling)  $<)
	@$(TRICK_CC) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(PYTHON_INCLUDES) -std=c++11 -Wno-invalid-offsetof -Wno-shadow -Wno-unused-but-set-variable -c -o $@ $<

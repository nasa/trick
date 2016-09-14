# This file can be used by a project to create a library containing the io_* and py_* code that
# Trick would normally generate during the simulation build process. Sims can then link against this
# library, reducing compilation time.
#
# To use it, your project should contain a directory that includes the following files:
#
# 1. S_source.hh
#    This file should include all header files for which you want io_* and py_* code generated.
#    This is not the file that Trick generates during compilation, but Trick's tools assume this
#    name, so you have to use it.
#
# 2. Makefile
#    You can name this file whatever you want, but it should include this makefile (the one you're
#    reading right now) and define the following variables:
#
#    LIBRARY_NAME
#    This determines the name of the generated library file. It is prepended with lib and appended
#    with _trick.a. Thus, the final name is lib$(LIBRARY_NAME)_trick.a
#
#    INCLUDE_FLAGS
#    Define this variable to specify the paths to the header files included by your S_source.hh.
#    For instance: -I$(HOME)/myproject/foo/include -I$(HOME)/myproject/bar/include
#
# Then, run make in that directory, and the library will be generated.
#
# Example Makefile:
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
# INCLUDE_FLAGS := -I$(HOME)/potato/include
# LIBRARY_NAME  := potato
#
# include $(TRICKIFY)

ifndef LIBRARY_NAME
    $(error LIBRARY_NAME must be set)
endif

ifndef INCLUDE_FLAGS
    $(error INCLUDE_FLAGS must be set)
endif

TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../../..)
OBJECTS    := $(shell find . -name "*.i")
OBJECTS    := $(join $(dir $(OBJECTS)),$(patsubst %.i,py_%.cpp,$(notdir $(OBJECTS))))
OBJECTS    += $(shell find . -name "*.cpp")
OBJECTS    := $(OBJECTS:.cpp=.o)
LIB        := lib$(LIBRARY_NAME)_trick.a

include $(TRICK_HOME)/share/trick/makefiles/Makefile.common
TRICK_CFLAGS   += $(INCLUDE_FLAGS)
TRICK_CXXFLAGS += $(INCLUDE_FLAGS)

# Ensure we can process all headers
undefine TRICK_EXT_LIB_DIRS

# While it would be nice to invoke the implicit archiving rule via a target like:
#
# $(LIB): $(LIB)($(OBJECTS))
#
# ar is not thread-safe, and, thus, we would have to disable parallelism via
# .NOTPARALLEL (a file-wide flag), significantly slowing the build process.

$(LIB): $(OBJECTS)
	@ar rsc $@ $?
	$(info Built $@)

# $(OBJECTS) is meant to contain all of the py_* and io_* object file names. We can't construct
# those until we run ICG and convert_swig. But we can't run the rules for ICG and convert_swig
# before $(OBJECTS) is expanded because it is a prerequiste of $(LIB), and prerequisites are always
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
# for $(LIB). To force this rule to always execute, we add a prerequisite on a non-existent file, a
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
# 13. $(LIB) can be executed with the correct prerequisites
#
# Note that build/Makefile_ICG is included after our main target since it contains targets itself,
# and we want make without an argument to build $(LIB).
#
# Note also that this executes ICG and convert_swig every time this file is processed, regardless
# of the target. Wasteful, but acceptable.

-include build/Makefile_ICG

build/Makefile_ICG: icg
	@$(TRICK_HOME)/libexec/trick/make_makefile_swig
	@$(TRICK_HOME)/libexec/trick/convert_swig

icg:
	@$(TRICK_HOME)/bin/trick-ICG $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) S_source.hh

clean:
	@rm -rf $(LIB) build

py_%.cpp: %.i
	$(info SWIGing $<)
	@$(SWIG) $(TRICK_INCLUDE) $(TRICK_DEFINES) $(TRICK_VERSIONS) -c++ -python -includeall -ignoremissing -w201,362,389,451 -o $@ $<

%.o: %.cpp
	$(info Compiling $<)
	@$(CXX) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(PYTHON_INCLUDES) -std=c++11 -Wno-invalid-offsetof -Wno-shadow -Wno-unused-but-set-variable -c -o $@ $<

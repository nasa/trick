# This file can be used to create an object file or library containing the io_*
# and py_* code that Trick would normally generate during the simulation build
# process. Sims can then link against that, reducing compilation time.
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
# TRICKIFY_BUILD_TYPE (optional)
# Valid options are:
# 1. STATIC (.a)
#    Create a static library. This will require the use of --whole-archive (on
#    Linux) or -all_load/-force_load (on Mac) when linking the sim executeable.
#    Trick uses dlsym to dynamically load symbols at run time, but the linker,
#    by default, will not include symbols from static libraries that are not
#    known to be needed at compile time.
# 2. SHARED (.so)
#    Create a shared object (dynamically linked library). This may require the
#    use of -rpath to ensure the linker can find the shared object at runtime
#    unless you explicitly link against it (as opposed to using -L and -l)
#    during compilation.
# 3. PLO (.o) [default]
#    Create a partially-linked object. No special linker options are required.
#
# TRICKIFY_OBJECT_NAME (optional)
# The name of the generated object file or library. The default value is
# trickified.o. You should choose something more meaningful, especially if
# you're using another build type.
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

TRICKIFY_BUILD_TYPE ?= PLO
TRICKIFY_OBJECT_NAME ?= trickified.o
TRICKIFY_PYTHON_DIR ?= python
TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../../..)
MAKE_OUT ?= build/MAKE_out

ifneq ($(wildcard build),)
    SWIG_OBJECTS := $(shell cat build/S_library_swig)
    SWIG_OBJECTS := $(addprefix build,$(addsuffix _py.o,$(basename $(SWIG_OBJECTS))))
    IO_OBJECTS := $(shell find build -name "io_*.cpp")
    IO_OBJECTS := $(IO_OBJECTS:.cpp=.o)
endif

include $(TRICK_HOME)/share/trick/makefiles/Makefile.common
TRICK_CFLAGS   += $(TRICKIFY_CXX_FLAGS)
TRICK_CXXFLAGS += $(TRICKIFY_CXX_FLAGS)

# Ensure we can process all headers
TRICK_EXT_LIB_DIRS :=

$(TRICKIFY_OBJECT_NAME): $(SWIG_OBJECTS) $(IO_OBJECTS) | $(dir $(TRICKIFY_OBJECT_NAME))
	$(info $(call COLOR,Linking)    $@)
ifeq ($(TRICKIFY_BUILD_TYPE),PLO)
	@echo $(LD) $(LD_PARTIAL) -o $@ $^ >> $(MAKE_OUT)
	$(ECHO_CMD)ld -r -o $@ $^ 2>&1 | $(TEE) -a $(MAKE_OUT); exit $${PIPESTATUS[0]}
else ifeq ($(TRICKIFY_BUILD_TYPE),SHARED)
	@echo $(TRICK_CPPC) -shared -o $@ $^ >> $(MAKE_OUT)
	$(ECHO_CMD)c++ -shared -o $@ $^ 2>&1 | $(TEE) -a $(MAKE_OUT); exit $${PIPESTATUS[0]}
else ifeq ($(TRICKIFY_BUILD_TYPE),STATIC)
	@echo ar rcs $@ $^ >> $(MAKE_OUT)
	$(ECHO_CMD)ar rcs $@ $^ 2>&1 | $(TEE) -a $(MAKE_OUT); exit $${PIPESTATUS[0]}
endif

$(dir $(TRICKIFY_OBJECT_NAME)) $(TRICKIFY_PYTHON_DIR) build:
	@mkdir -p $@

$(IO_OBJECTS): %.o: %.cpp
	$(info $(call COLOR,Compiling)  $<)
	@echo $(TRICK_CPPC) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) -std=c++11 -Wno-invalid-offsetof -MMD -MP -c -o $@ $< >> $(MAKE_OUT)
	$(ECHO_CMD)$(TRICK_CPPC) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) -std=c++11 -Wno-invalid-offsetof -MMD -MP -c -o $@ $< 2>&1 | $(TEE) -a $(MAKE_OUT) ; exit $${PIPESTATUS[0]}

$(IO_OBJECTS:.o=.d): %.d: ;

-include $(IO_OBJECTS:.o=.d)

$(SWIG_OBJECTS): %.o: %.cpp
	$(info $(call COLOR,Compiling)  $<)
	@echo $(TRICK_CPPC) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(PYTHON_INCLUDES) -Wno-unused-parameter -Wno-shadow -c -o $@ $< >> $(MAKE_OUT)
	$(ECHO_CMD)$(TRICK_CPPC) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(PYTHON_INCLUDES) -Wno-unused-parameter -Wno-shadow -c -o $@ $< 2>&1 | $(TEE) -a $(MAKE_OUT) ; exit $${PIPESTATUS[0]}

$(SWIG_OBJECTS:.o=.cpp): %.cpp: %.i | $(TRICKIFY_PYTHON_DIR) $(SWIG_OBJECTS:.o=.i)
	$(info $(call COLOR,SWIGing)    $<)
	@echo $(SWIG) $(TRICK_INCLUDE) $(TRICK_DEFINES) $(TRICK_VERSIONS) $(TRICK_SWIG_FLAGS) -c++ -python -includeall -ignoremissing -w201,303,325,362,389,401,451 -outdir $(TRICKIFY_PYTHON_DIR) -o $@ $< >> $(MAKE_OUT)
	$(ECHO_CMD)$(SWIG) $(TRICK_INCLUDE) $(TRICK_DEFINES) $(TRICK_VERSIONS) $(TRICK_SWIG_FLAGS) -c++ -python -includeall -ignoremissing -w201,303,325,362,389,401,451 -outdir $(TRICKIFY_PYTHON_DIR) -o $@ $< 2>&1 | $(TEE) -a $(MAKE_OUT) ; exit $${PIPESTATUS[0]}

define create_convert_swig_rule
build/%_py.i: /%.$1
	$$(info $$(call COLOR,Converting) $$<)
	@echo ${TRICK_HOME}/$(LIBEXEC)/trick/convert_swig ${TRICK_CONVERT_SWIG_FLAGS} $$< >> $(MAKE_OUT)
	$(ECHO_CMD)${TRICK_HOME}/$(LIBEXEC)/trick/convert_swig ${TRICK_CONVERT_SWIG_FLAGS} $$< 2>&1 | $(TEE) -a $(MAKE_OUT) ; exit $$${PIPESTATUS[0]}
endef

EXTENSIONS := H h hh hxx h++ hpp
$(foreach EXTENSION,$(EXTENSIONS),$(eval $(call create_convert_swig_rule,$(EXTENSION))))

# SWIG_OBJECTS and IO_OBJECTS are meant to contain all of the *_py and io_*
# object file names, respectively, by looking at products of ICG and
# make_makefile_swig. However, we can't run a rule for ICG before those
# variables are expanded because they are prerequisites of
# TRICKIFY_OBJECT_NAME, and prerequisites are immediately expanded.
# Therefore, when make processes this file (for the first time after a clean),
# the variables are empty because the files from which they are populated have
# yet to be created by ICG. What we really want is to run ICG before OBJECTS is
# expanded, but only if we need to.
#
# We can do this by taking advantage of (abusing) make's behavior in the
# presence of included files. When you include another file, make attempts to
# update that file, and then re-executes the original makefile with a clean
# slate if any of the included files changed. So we need a rule that:
#
# 1. Executes only if S_source.hh or anything in its include tree changes.
# 2. Runs ICG and make_makefile_swig to generate io_* files and S_library_swig.
# 3. Updates its dependency file.
#
# We can then include the dependency file to trigger make to re-execute this
# main makefile if changes were detected, thus updating SWIG_OBJECTS and
# IO_OBJECTS to the latest results of ICG and make_makefile_swig.
#
# gcc's option to automatically generate dependency information is exactly
# what we need. It will produce a file containing a make rule whose
# dependencies are everything in S_source.hh's include tree plus S_source.hh
# itself. By telling gcc to make the target of the rule the same target we use
# to execute the gcc call, we end up with a target that maintains its own
# dependency list. The method is laid out in more detail here:
# http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/

build/S_source.d: | $(dir $@)
	$(ECHO_CMD)$(TRICK_HOME)/bin/trick-ICG $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(TRICK_ICGFLAGS) S_source.hh
	$(ECHO_CMD)$(TRICK_HOME)/$(LIBEXEC)/trick/make_makefile_swig 2>&1 | $(TEE) -a $(MAKE_OUT) ; exit $${PIPESTATUS[0]}
	@echo $(TRICK_CC) -MM -MP -MT $@ -MF $@ $(TRICKIFY_CXX_FLAGS) S_source.hh >> $(MAKE_OUT)
	$(ECHO_CMD) $(TRICK_CC) -MM -MP -MT $@ -MF $@ $(TRICKIFY_CXX_FLAGS) S_source.hh 2>&1 | $(TEE) -a $(MAKE_OUT) ; exit $${PIPESTATUS[0]}

-include build/S_source.d

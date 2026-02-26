# We know this file's position relative to the root directory of the project,
# and MAKEFILE_LIST will give us the full path to this file no matter where the
# user has installed this project.
export MYPROJECT_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/..)

# Specify include paths for your headers.
MYPROJECT_INCLUDE := -I$(MYPROJECT_HOME)/include_bar -I$(MYPROJECT_HOME)/include_foo

# Users may set different flags for C and C++, so you should really modify both
# to be safe.
TRICK_CFLAGS   += $(MYPROJECT_INCLUDE) $(MYPROJECT_SOURCE)
TRICK_CXXFLAGS += $(MYPROJECT_INCLUDE) $(MYPROJECT_SOURCE)

export TRICKIFY_OBJECT_NAME := trickified_myproject.o
MYPROJECT_TRICK := $(MYPROJECT_HOME)/trickified/$(TRICKIFY_OBJECT_NAME)

TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../../..)

# set CONFIG_MK to allow compilation without running configure
CONFIG_MK = 1

include ${TRICK_HOME}/share/trick/makefiles/Makefile.common
# set the TRICK_LIB variable to create a separate library for comm
TRICK_LIB := $(TRICK_LIB_DIR)/libtrick_connection_handlers.a
include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib
-include Makefile_deps

TRICK_CXXFLAGS +=

# make the comm library when called by the master makefile.
trick: ${TRICK_LIB}

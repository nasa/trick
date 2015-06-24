# set CONFIG_MK to allow compilation without running configure
CONFIG_MK = 1

include ${TRICK_HOME}/share/trick/makefiles/Makefile.common
# set the TRICK_LIB variable to create a separate library
TRICK_LIB := $(TRICK_LIB_DIR)/libtrick_units.a
include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib
-include Makefile_deps

# make the library when called by the master makefile.
trick: ${TRICK_LIB}

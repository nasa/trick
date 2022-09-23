# set CONFIG_MK to allow compilation without running configure
CONFIG_MK = 1

include $(dir $(lastword $(MAKEFILE_LIST)))../../../share/trick/makefiles/Makefile.common
# set the TRICK_LIB variable to create a separate library for the Memory Manager.
TRICK_LIB := $(TRICK_LIB_DIR)/libtrick_mm.a
include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib
-include Makefile_deps

# make the Memory Manager library when called by the master makefile.
trick: ${TRICK_LIB}

#include ${TRICK_HOME}/share/trick/makefiles/Makefile.common
#include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib
#-include Makefile_deps
#
#$(L_OBJS) : TRICK_CXXFLAGS += -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -x c++
#$(Y_OBJS) : TRICK_CXXFLAGS += -Wno-unused-parameter -Wno-unused -x c++
#
#ifeq ($(IS_CC_CLANG), 1)
#$(Y_OBJS) : TRICK_CXXFLAGS += -Wno-parentheses-equality
#endif
#
## suppress warnings files with shadowing.
#$(CPP_OBJS) : TRICK_CXXFLAGS += -Wno-shadow
#

docs: MM.doxygen
	doxygen MM.doxygen

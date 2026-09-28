# set CONFIG_MK to allow compilation without running configure
CONFIG_MK = 1

include $(dir $(lastword $(MAKEFILE_LIST)))../../../share/trick/makefiles/Makefile.common
# set the TRICK_LIB variable to create a separate library for the Memory Manager.
TRICK_LIB := $(TRICK_LIB_DIR)/libtrick_mm.a
include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib
-include Makefile_deps

# adef_parser.y/ref_parser.y no longer set %name-prefix (deprecated since
# bison 2.6, warned about since bison 3.3), so pass the prefix on the
# command line instead.
adef_parser.tab.cpp : YACC_FLAGS += -p ADEF_
ref_parser.tab.cpp : YACC_FLAGS += -p REF_

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

include $(dir $(lastword $(MAKEFILE_LIST)))../../../share/trick/makefiles/Makefile.common
include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib
-include Makefile_deps

# suppress warnings in lex and yacc files.
$(L_OBJS) : TRICK_CXXFLAGS += -Wno-unused-parameter -Wno-unused-function -Wno-sign-compare -x c++
$(Y_OBJS) : TRICK_CXXFLAGS += -Wno-unused-parameter -Wno-unused -x c++

ifeq ($(IS_CC_CLANG), 1)
$(Y_OBJS) : TRICK_CXXFLAGS += -Wno-parentheses-equality
endif

# suppress warnings files with shadowing.
$(CPP_OBJS) : TRICK_CXXFLAGS += -Wno-shadow

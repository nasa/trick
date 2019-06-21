include $(dir $(lastword $(MAKEFILE_LIST)))../../../share/trick/makefiles/Makefile.common
include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib

-include Makefile_deps

TRICK_CXXFLAGS += -std=c++11

ifeq ($(HAVE_GSL),1)
TRICK_CXXFLAGS += -D_HAVE_GSL
ifneq ($(GSL_HOME),/usr)
TRICK_CXXFLAGS += -I${GSL_HOME}/include
endif
endif

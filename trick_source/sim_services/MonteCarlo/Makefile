
# $Id$

include ${TRICK_HOME}/makefiles/Makefile.common
include ${TRICK_HOME}/makefiles/Makefile.tricklib

-include Makefile_deps

ifeq ($(HAVE_GSL),1)
TRICK_CXXFLAGS += -D_HAVE_GSL
ifneq ($(GSL_HOME),/usr)
TRICK_CXXFLAGS += -I${GSL_HOME}/include
endif
endif


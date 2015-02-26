
# $Id: Makefile 3026 2013-06-17 18:37:13Z dstrauss $

include ${TRICK_HOME}/makefiles/Makefile.common
include ${TRICK_HOME}/makefiles/Makefile.tricklib
-include Makefile_deps

ifneq ($(DMTCP),)
TRICK_CXXFLAGS += -D_DMTCP -I$(DMTCP)/dmtcpaware
endif

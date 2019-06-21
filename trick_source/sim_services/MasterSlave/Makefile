include $(dir $(lastword $(MAKEFILE_LIST)))../../../share/trick/makefiles/Makefile.common
include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib
-include Makefile_deps

ifneq ($(DMTCP),)
TRICK_CXXFLAGS += -D_DMTCP -I$(DMTCP)/dmtcpaware
endif

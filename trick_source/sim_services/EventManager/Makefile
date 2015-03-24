
include ${TRICK_HOME}/makefiles/Makefile.common
include ${TRICK_HOME}/makefiles/Makefile.tricklib

TRICK_CFLAGS += $(PYTHON_INCLUDES)
TRICK_CXXFLAGS += $(PYTHON_INCLUDES)

ifeq ($(PYTHON_VERSION),2.4)
 TRICK_CXXFLAGS += -DPYTHON_PROTECT_HANG
endif

-include Makefile_deps


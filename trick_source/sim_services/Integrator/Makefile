
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common
include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib
-include Makefile_deps

ifeq ($(USE_ER7_UTILS_INTEGRATORS), 0)
trick:
	$(MAKE) -C trick_algorithms
endif

clean_algs:
	$(MAKE) -C trick_algorithms real_clean

real_clean: clean_algs

# source files do not have usual relative path, so we have to add this to get "make depend" to work
depend: TRICK_CXXFLAGS += -Iinclude


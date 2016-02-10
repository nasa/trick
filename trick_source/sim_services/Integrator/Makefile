
include ${TRICK_HOME}/makefiles/Makefile.common
include ${TRICK_HOME}/makefiles/Makefile.tricklib
-include Makefile_deps

ifeq ($(USE_ER7_UTILS), 1)
trick:
	@echo "#####################################################################"
	@echo "##                      Make ER7 ALGORITHMS                        ##"
	@echo "#####################################################################"
	$(MAKE) -C er7_algorithms
else
trick:
	@echo "#####################################################################"
	@echo "##                     Make Trick ALGORITHMS                       ##"
	@echo "#####################################################################"
	$(MAKE) -C trick_algorithms
endif

clean_algs:
	@echo "#####################################################################"
	@echo "##                     Clean Integ ALGORITHMS                      ##"
	@echo "#####################################################################"
	$(MAKE) -C er7_algorithms real_clean
	$(MAKE) -C trick_algorithms real_clean

real_clean: clean_algs

# source files do not have usual relative path, so we have to add this to get "make depend" to work
depend: TRICK_CXXFLAGS += -Iinclude


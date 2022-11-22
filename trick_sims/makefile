TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
export TRICK_HOST_CPU := $(shell $(TRICK_HOME)/bin/trick-gte TRICK_HOST_CPU)
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common


NESTED_SIM_DIRECTORIES = $(wildcard */SIM_*)
SIM_DIRECTORIES = $(wildcard SIM_*) $(NESTED_SIM_DIRECTORIES)

UNIT_TEST_RESULTS = $(addprefix $(TRICK_HOME)/trick_test/, $(addsuffix .xml, $(SIM_DIRECTORIES)))


clean:
	rm -f $(UNIT_TEST_RESULTS)
	- for i in $(SIM_DIRECTORIES) ; do \
		if [ -f "$$i/"[Mm]"akefile" ] ; then \
			$(MAKE) -C $$i spotless ; \
		fi \
	done

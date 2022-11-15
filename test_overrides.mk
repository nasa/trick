TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
export TRICK_HOST_CPU := $(shell $(TRICK_HOME)/bin/trick-gte TRICK_HOST_CPU)
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

# Remove when issue #1147 is fixed: https://github.com/nasa/trick/issues/1147
unexport TRICK_PYTHON_PATH

sim_test:
	python3 trickops.py ${TRICK_HOME}

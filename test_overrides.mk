TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))
export TRICK_HOST_CPU := $(shell $(TRICK_HOME)/bin/trick-gte TRICK_HOST_CPU)
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common


unexport TRICK_PYTHON_PATH

sim_test:
	python3 trickops.py ${TRICK_HOME}

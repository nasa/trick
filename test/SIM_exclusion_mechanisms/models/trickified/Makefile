SIM_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../..)
TRICK_HOME := $(abspath $(SIM_HOME)/../..)
export TRICKIFY_CXX_FLAGS := -I$(SIM_HOME)/models

all:
	@$(MAKE) -s -f $(TRICK_HOME)/share/trick/makefiles/trickify.mk

clean:
	@rm -rf build python trick trickified.o

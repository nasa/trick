
export TRICK_HOST_CPU := $(shell $(TRICK_HOME)/bin/trick-gte TRICK_HOST_CPU)

# List out sims we want to compile
COMPILE_DIRS = \
    Ball/SIM_ball_L1 \
    Ball/SIM_ball_L2 \
    Ball/SIM_ball_L3 \
    Cannon/SIM_amoeba \
    Cannon/SIM_cannon_aero \
    Cannon/SIM_cannon_analytic \
    Cannon/SIM_cannon_eulercromer \
    Cannon/SIM_cannon_numeric \
    Cannon/SIM_cannon_jet \
    SIM_Ball++_L1 \
    SIM_satellite \
    SIM_sun

    #SIM_monte \

# List out sims we want to run unit tests
TEST_DIRS = \
    Ball/SIM_ball_L1 \
    Ball/SIM_ball_L2 \
    Ball/SIM_ball_L3 \
    Cannon/SIM_amoeba \
    Cannon/SIM_cannon_aero \
    Cannon/SIM_cannon_analytic \
    Cannon/SIM_cannon_eulercromer \
    Cannon/SIM_cannon_numeric \
    Cannon/SIM_cannon_jet \
    SIM_Ball++_L1 \
    SIM_sun

    #SIM_monte \

EXECUTABLES = $(addsuffix /T_main_${TRICK_HOST_CPU}_test.exe, $(COMPILE_DIRS))
UNIT_TEST_RESULTS = $(addprefix $(TRICK_HOME)/trick_test/, $(addsuffix .xml, $(TEST_DIRS)))

all:
	@echo "This makefile is used with Trick's top level 'make test' command"

test: $(EXECUTABLES) $(UNIT_TEST_RESULTS)

clean:
	rm -f $(UNIT_TEST_RESULTS)
	- for i in $(COMPILE_DIRS) ; do \
          if [ -f "$$i/"[Mm]"akefile" ] ; then \
              $(MAKE) -C $$i spotless ; \
          fi \
      done

$(EXECUTABLES):
	@ cd $(@D) ; ${TRICK_HOME}/bin/trick-CP -t ;

$(UNIT_TEST_RESULTS): $(TRICK_HOME)/trick_test/%.xml : %/T_main_${TRICK_HOST_CPU}_test.exe
	@ cd $* ; ./T_main_${TRICK_HOST_CPU}_test.exe RUN_test/unit_test.py



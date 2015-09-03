
export TRICK_HOST_CPU := $(shell $(TRICK_HOME)/bin/trick-gte TRICK_HOST_CPU)

# List out sims we want to compile
COMPILE_DIRS = SIM_Ball++_L1 \
    SIM_amoeba \
    SIM_ball_L1 \
    SIM_ball_L2 \
    SIM_ball_L3 \
    SIM_ball_default_data \
    SIM_cannon_analytic \
    SIM_cannon_contact \
    SIM_cannon_dt \
    SIM_cannon_eulercromer \
    SIM_cannon_integ \
    SIM_demo_inputfile \
    SIM_demo_sdefine \
    SIM_events \
    SIM_jet_call \
    SIM_jet_event \
    SIM_jet_read \
    SIM_monte \
    SIM_rti \
    SIM_satellite \
    SIM_stls \
    SIM_stls2 \
    SIM_sun \
    SIM_target \
    SIM_tcltk_java \
    SIM_test_abstract \
    SIM_test_dp \
    SIM_test_dr \
    SIM_test_inherit \
    SIM_test_ip \
    SIM_test_ip2 \
    SIM_test_sched \
    SIM_test_simlib \
    SIM_test_templates \
    SIM_threads \
    SIM_threads_simple \
    SIM_trickcomm

# This test is temporarily sitting out until fixed.
#    SIM_test_varserv

# List out sims we want to run unit tests
TEST_DIRS = SIM_Ball++_L1 \
    SIM_amoeba \
    SIM_ball_L1 \
    SIM_ball_L2 \
    SIM_ball_L3 \
    SIM_ball_default_data \
    SIM_cannon_analytic \
    SIM_cannon_contact \
    SIM_cannon_dt \
    SIM_cannon_eulercromer \
    SIM_cannon_integ \
    SIM_demo_sdefine \
    SIM_events \
    SIM_jet_call \
    SIM_jet_event \
    SIM_jet_read \
    SIM_monte \
    SIM_rti \
    SIM_stls \
    SIM_stls2 \
    SIM_sun \
    SIM_target \
    SIM_test_dp \
    SIM_test_dr \
    SIM_test_ip \
    SIM_test_sched \
    SIM_test_simlib \
    SIM_test_templates \
    SIM_threads

# This test is temporarily sitting out until fixed.
#    SIM_test_varserv

EXECUTABLES = $(addsuffix /T_main_${TRICK_HOST_CPU}_test.exe, $(COMPILE_DIRS))
UNIT_TEST_RESULTS = $(addprefix $(TRICK_HOME)/trick_test/, $(addsuffix .xml, $(TEST_DIRS)))

all:
	@echo "This makefile is used with Trick's top level 'make test' command"

test: $(EXECUTABLES) $(UNIT_TEST_RESULTS) data_record_results

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

DR_RESULTS = $(TRICK_HOME)/trick_sims/SIM_test_dr/RUN_test
data_record_results: $(UNIT_TEST_RESULTS)
	diff $(DR_RESULTS)/log_DR_bitfieldsASCII.csv $(DR_RESULTS)/Ref_Logs/log_DR_bitfieldsASCII_Master.csv
	diff $(DR_RESULTS)/log_DR_typesASCII.csv $(DR_RESULTS)/Ref_Logs/log_DR_typesASCII_Master.csv
ifneq ($(wildcard $(DR_RESULTS)/Ref_Logs/typesB_${TRICK_HOST_CPU}.trk), )
	cmp -b $(DR_RESULTS)/log_DR_bitfieldsBINARY.trk $(DR_RESULTS)/Ref_Logs/bitfB_${TRICK_HOST_CPU}.trk
	cmp -b $(DR_RESULTS)/log_DR_typesBINARY.trk $(DR_RESULTS)/Ref_Logs/typesB_${TRICK_HOST_CPU}.trk
endif



TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
export TRICK_HOST_CPU := $(shell $(TRICK_HOME)/bin/trick-gte TRICK_HOST_CPU)

# List out sims we want to compile
COMPILE_DIRS = \
	SIM_demo_sdefine \
	SIM_events \
	SIM_rti \
	SIM_stls \
	SIM_test_dp \
	SIM_test_dr \
	SIM_test_io \
	SIM_test_ip \
	SIM_test_sched \
	SIM_test_templates \
	SIM_threads

SIMS_NEEDING_TEST = \
	SIM_alloc_test \
	SIM_demo_inputfile \
	SIM_measurement_units \
	SIM_test_abstract \
	SIM_test_inherit \
	SIM_test_ip2 \
	SIM_threads_simple \
	SIM_trickcomm

# Sims with problems, no purpose, or maybe shouldn't be a test
# SIM_leaks ( should be deleted )
# SIM_test_varserv  ( not sure what it is testing )
# SIM_dynamic_sim_object ( not running, class won't instantiate )
# SIM_segments  ( not a test, but a demo )

# This test is temporarily sitting out until fixed.
#    SIM_test_varserv

EXECUTABLES = $(addsuffix /T_main_${TRICK_HOST_CPU}_test.exe, $(COMPILE_DIRS) $(SIMS_NEEDING_TEST))
UNIT_TEST_RESULTS = $(addprefix $(TRICK_HOME)/trick_test/, $(addsuffix .xml, $(COMPILE_DIRS)))

test: $(EXECUTABLES) $(UNIT_TEST_RESULTS) data_record_results

clean:
	rm -f $(UNIT_TEST_RESULTS)
	- for i in $(COMPILE_DIRS) ; do \
		if [ -f "$$i/"[Mm]"akefile" ] ; then \
			$(MAKE) -C $$i spotless ; \
		fi \
	done

	- for i in $(SIMS_NEEDING_TEST) ; do \
		if [ -f "$$i/"[Mm]"akefile" ] ; then \
			$(MAKE) -C $$i spotless ; \
		fi \
	done

$(EXECUTABLES):
	@ cd $(@D) ; ${TRICK_HOME}/bin/trick-CP -t

$(UNIT_TEST_RESULTS): $(TRICK_HOME)/trick_test/%.xml : %/T_main_${TRICK_HOST_CPU}_test.exe
	@ cd $* ; ./T_main_${TRICK_HOST_CPU}_test.exe RUN_test/unit_test.py

DR_RESULTS = $(TRICK_HOME)/test/SIM_test_dr/RUN_test
data_record_results: $(TRICK_HOME)/trick_test/SIM_test_dr.xml $(DR_RESULTS)
	diff $(DR_RESULTS)/log_DR_bitfieldsASCII.csv $(DR_RESULTS)/Ref_Logs/log_DR_bitfieldsASCII_Master.csv
	diff $(DR_RESULTS)/log_DR_typesASCII.csv $(DR_RESULTS)/Ref_Logs/log_DR_typesASCII_Master.csv
	cmp -b $(DR_RESULTS)/log_DR_bitfieldsBINARY.trk $(DR_RESULTS)/Ref_Logs/log_DR_bitfieldsBINARY.trk
	cmp -b $(DR_RESULTS)/log_DR_typesBINARY.trk $(DR_RESULTS)/Ref_Logs/log_DR_typesBINARY.trk


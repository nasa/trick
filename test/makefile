TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))..)
export TRICK_HOST_CPU := $(shell $(TRICK_HOME)/bin/trick-gte TRICK_HOST_CPU)
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

# Remove when issue #1147 is fixed: https://github.com/nasa/trick/issues/1147
unexport TRICK_PYTHON_PATH

SIMS_TO_COMPILE_ONLY = \
	SIM_alloc_test \
	SIM_default_member_initializer \
	SIM_delete_default_constructor \
	SIM_demo_inputfile \
	SIM_exclusion_mechanisms \
	SIM_isystem \
	SIM_measurement_units \
	SIM_parse_s_define \
	SIM_target_specific_variables \
	SIM_test_abstract \
	SIM_test_inherit \
	SIM_test_ip2 \
	SIM_threads_simple \
	SIM_trickcomm

SIMS_TO_COMPILE_AND_RUN = \
	SIM_demo_sdefine \
	SIM_events \
	SIM_python_namespace \
	SIM_rti \
	SIM_stls \
	SIM_test_dp \
	SIM_test_dr \
	SIM_test_icg_file_skipped \
	SIM_test_io \
	SIM_test_ip \
	SIM_test_sched \
	SIM_test_templates \
	SIM_threads \
	SIM_trickified

# Sims with problems, no purpose, or maybe shouldn't be a test
# SIM_leaks ( should be deleted )
# SIM_test_varserv  ( not sure what it is testing )
# SIM_dynamic_sim_object ( not running, class won't instantiate )
# SIM_segments  ( not a test, but a demo )

# This test is temporarily sitting out until fixed.
#    SIM_test_varserv

EXECUTABLES = $(addsuffix /T_main_${TRICK_HOST_CPU}_test.exe, $(SIMS_TO_COMPILE_AND_RUN) $(SIMS_TO_COMPILE_ONLY))
UNIT_TEST_RESULTS = $(addprefix $(TRICK_HOME)/trick_test/, $(addsuffix .xml, $(SIMS_TO_COMPILE_AND_RUN)))

test: $(EXECUTABLES) $(UNIT_TEST_RESULTS) data_record_results

clean:
	rm -f $(UNIT_TEST_RESULTS)
	- for i in $(SIMS_TO_COMPILE_AND_RUN) ; do \
		if [ -f "$$i/"[Mm]"akefile" ] ; then \
			$(MAKE) -C $$i spotless ; \
		fi \
	done

	- for i in $(SIMS_TO_COMPILE_ONLY) ; do \
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
ifeq (${TRICK_FORCE_32BIT}, 1)
	cmp -b $(DR_RESULTS)/log_DR_typesBINARY.trk $(DR_RESULTS)/Ref_Logs/log_DR_typesBINARY_32.trk
else
	cmp -b $(DR_RESULTS)/log_DR_typesBINARY.trk $(DR_RESULTS)/Ref_Logs/log_DR_typesBINARY.trk
endif

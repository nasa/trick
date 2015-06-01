
# Makefile to build Trick

# Make Targets
#-------------------------------------------------------------------------------
# default   - Compile Trick-core and Trick Data-products Libraries and Applications.
# 'no_dp'   - Compile Trick-core only.
# 'dp'      - Compile Data-products only.
# 'java'    - Compile Java GUI Applications
# 'javadoc' - Generate Java Documentation.
# 'doxygen' - Generate HTML User's Guide.
# 'test'    - Run Unit-tests and Simulation Tests.

export TRICK_HOME = $(CURDIR)

# Include the build configuration information.
include $(TRICK_HOME)/share/trick/makefiles/Makefile.common

#-------------------------------------------------------------------------------
# Specify the contents of: libtrick.a
#-------------------------------------------------------------------------------
TRICK_LIB = $(TRICK_LIB_DIR)/libtrick.a
SIM_SERV_DIRS = \
	${TRICK_HOME}/trick_source/sim_services/ExternalApplications \
	${TRICK_HOME}/trick_source/sim_services/Clock \
	${TRICK_HOME}/trick_source/sim_services/CheckPointAgent \
	${TRICK_HOME}/trick_source/sim_services/CheckPointRestart \
	${TRICK_HOME}/trick_source/sim_services/Collect \
	${TRICK_HOME}/trick_source/sim_services/CommandLineArguments \
	${TRICK_HOME}/trick_source/sim_services/DataRecord \
	${TRICK_HOME}/trick_source/sim_services/DebugPause \
	${TRICK_HOME}/trick_source/sim_services/DMTCP \
	${TRICK_HOME}/trick_source/sim_services/EchoJobs \
	${TRICK_HOME}/trick_source/sim_services/Environment \
	${TRICK_HOME}/trick_source/sim_services/EventManager \
	${TRICK_HOME}/trick_source/sim_services/Executive \
	${TRICK_HOME}/trick_source/sim_services/FrameLog \
	${TRICK_HOME}/trick_source/sim_services/JITInputFile \
	${TRICK_HOME}/trick_source/sim_services/JSONVariableServer \
	${TRICK_HOME}/trick_source/sim_services/Integrator \
	${TRICK_HOME}/trick_source/sim_services/UnitTest \
	${TRICK_HOME}/trick_source/sim_services/MasterSlave \
	${TRICK_HOME}/trick_source/sim_services/MemoryManager \
	${TRICK_HOME}/trick_source/sim_services/Message \
	${TRICK_HOME}/trick_source/sim_services/MonteCarlo \
	${TRICK_HOME}/trick_source/sim_services/RealtimeInjector \
	${TRICK_HOME}/trick_source/sim_services/RealtimeSync \
	${TRICK_HOME}/trick_source/sim_services/STL \
	${TRICK_HOME}/trick_source/sim_services/ScheduledJobQueue \
	${TRICK_HOME}/trick_source/sim_services/Scheduler \
	${TRICK_HOME}/trick_source/sim_services/Sie \
	${TRICK_HOME}/trick_source/sim_services/SimObject \
	${TRICK_HOME}/trick_source/sim_services/SimTime \
	${TRICK_HOME}/trick_source/sim_services/ThreadBase \
	${TRICK_HOME}/trick_source/sim_services/Timer \
	${TRICK_HOME}/trick_source/sim_services/Units \
	${TRICK_HOME}/trick_source/sim_services/VariableServer \
	${TRICK_HOME}/trick_source/sim_services/Zeroconf \
	${TRICK_HOME}/trick_source/sim_services/include \
	${TRICK_HOME}/trick_source/sim_services/mains \
	${TRICK_HOME}/include/trick

SIM_SERV_OBJS = $(addsuffix /object_$(TRICK_HOST_CPU)/*.o ,$(SIM_SERV_DIRS))

ER7_UTILS_DIRS = \
	${ER7_UTILS_HOME}/integration/abm4 \
	${ER7_UTILS_HOME}/integration/beeman \
	${ER7_UTILS_HOME}/integration/core \
	${ER7_UTILS_HOME}/integration/euler \
	${ER7_UTILS_HOME}/integration/mm4 \
	${ER7_UTILS_HOME}/integration/nl2 \
	${ER7_UTILS_HOME}/integration/position_verlet \
	${ER7_UTILS_HOME}/integration/rk2_heun \
	${ER7_UTILS_HOME}/integration/rk2_midpoint \
	${ER7_UTILS_HOME}/integration/rk4 \
	${ER7_UTILS_HOME}/integration/rkf45 \
	${ER7_UTILS_HOME}/integration/rkf78 \
	${ER7_UTILS_HOME}/integration/rkg4 \
	${ER7_UTILS_HOME}/integration/symplectic_euler \
	${ER7_UTILS_HOME}/integration/velocity_verlet \
	${ER7_UTILS_HOME}/interface \
	${ER7_UTILS_HOME}/math \
	${ER7_UTILS_HOME}/trick/integration
ER7_UTILS_OBJS = $(addsuffix /object_$(TRICK_HOST_CPU)/*.o ,$(ER7_UTILS_DIRS))

UTILS_DIRS = \
	${TRICK_HOME}/trick_source/trick_utils/interpolator \
	${TRICK_HOME}/trick_source/trick_utils/trick_adt \
	${TRICK_HOME}/trick_source/trick_utils/comm \
	${TRICK_HOME}/trick_source/trick_utils/shm \
	${TRICK_HOME}/trick_source/trick_utils/math \
	${TRICK_HOME}/trick_source/trick_utils/units
UTILS_OBJS = $(addsuffix /object_$(TRICK_HOST_CPU)/*.o ,$(UTILS_DIRS))

#-------------------------------------------------------------------------------
# Specify the contents of: libtrick_pyip.a
# This library contains the SWIG generated interface-code between Trick and Python.
#-------------------------------------------------------------------------------
TRICK_SWIG_LIB = $(TRICK_LIB_DIR)/libtrick_pyip.a

SWIG_DIRS = \
	${TRICK_HOME}/trick_source/sim_services/InputProcessor \
	${TRICK_HOME}/trick_source/trick_swig

SWIG_OBJS = $(addsuffix /object_$(TRICK_HOST_CPU)/*.o ,$(SWIG_DIRS))

#-------------------------------------------------------------------------------
# Specify where to find units tests.
#-------------------------------------------------------------------------------
UNIT_TEST_DIRS := \
    $(wildcard ${TRICK_HOME}/trick_source/sim_services/*/test) \
    $(wildcard ${TRICK_HOME}/trick_source/trick_utils/*/test) \
    ${TRICK_HOME}/trick_source/data_products/DPX/test/unit_test
ifeq ($(USE_ER7_UTILS_INTEGRATORS), 0)
  UNIT_TEST_DIRS := $(filter-out %Integrator/test,$(UNIT_TEST_DIRS))
endif

#-------------------------------------------------------------------------------
# FIXME:
# This is only used by the 'clean_test' target below. Seems to me that it really
# doesn't belong here. Instead, the 'clean' target in
# $TRICK_HOME/trick_sims/makefile define this.
#-------------------------------------------------------------------------------
MODEL_DIRS = \
	${TRICK_HOME}/trick_models/Ball++/L1 \
	${TRICK_HOME}/trick_models/ball/L1 \
	${TRICK_HOME}/trick_models/ball/L2 \
	${TRICK_HOME}/trick_models/baseball/aero \
	${TRICK_HOME}/trick_models/baseball/optim \
	${TRICK_HOME}/trick_models/cannon/aero \
	${TRICK_HOME}/trick_models/cannon/graphics \
	${TRICK_HOME}/trick_models/cannon/gravity \
	${TRICK_HOME}/trick_models/cannon/optim \
	${TRICK_HOME}/trick_models/exclude_me \
	${TRICK_HOME}/trick_models/helios \
	${TRICK_HOME}/trick_models/stl_checkpoint \
	${TRICK_HOME}/trick_models/target \
	${TRICK_HOME}/trick_models/test/dp \
	${TRICK_HOME}/trick_models/test/impexp \
	${TRICK_HOME}/trick_models/test/ip \
	${TRICK_HOME}/trick_models/test/ip2 \
	${TRICK_HOME}/trick_models/test/sched \
	${TRICK_HOME}/trick_models/test_ip \
	${TRICK_HOME}/trick_models/threads

# The name of the ICG executable indicates the operating system, and the machine
# hardware on which it is built. This allows pre-build ICG binaries to be
# distributed in the installation package. The reason for distributing pre-built
# ICG binaries is because the user's machine may not have the requisite clang
# libraries.
ICG_EXE := ${TRICK_HOME}/bin/trick-ICG

################################################################################
#                                   RULES
################################################################################
# DEFAULT TARGET
# 1 Build Trick-core and Trick Data-products.
all: no_dp dp java
	@ echo ; echo "[32mTrick compilation complete:[00m" ; date

#-------------------------------------------------------------------------------
# 1.1 Build Trick-core
no_dp: $(TRICK_LIB) $(TRICK_SWIG_LIB)
	@ echo ; echo "Trick libs compiled:" ; date

# 1.1.1 Build libTrick.a
ifeq ($(USE_ER7_UTILS_INTEGRATORS), 1)
$(TRICK_LIB): $(SIM_SERV_DIRS) $(ER7_UTILS_DIRS) $(UTILS_DIRS) | $(TRICK_LIB_DIR)
	ar crs $@ $(SIM_SERV_OBJS) $(ER7_UTILS_OBJS) $(UTILS_OBJS)
else
$(TRICK_LIB): $(SIM_SERV_DIRS) $(UTILS_DIRS) | $(TRICK_LIB_DIR)
	ar crs $@ $(SIM_SERV_OBJS) $(UTILS_OBJS)
endif

# 1.1.1.1 Compile the objects in the specified sim_services directories.
.PHONY: $(SIM_SERV_DIRS)
$(SIM_SERV_DIRS): icg_sim_serv $(TRICK_LIB_DIR)
	@ $(MAKE) -C $@ trick

# 1.1.1.2 Compile the objects in the specified utils directories.
.PHONY: $(UTILS_DIRS)
$(UTILS_DIRS): icg_sim_serv
	@ $(MAKE) -C $@ trick

# 1.1.1.3 Compile the objects in the specified er7_utils directories.
.PHONY: $(ER7_UTILS_DIRS)
$(ER7_UTILS_DIRS): TRICK_CXXFLAGS += -Wno-unused-parameter
$(ER7_UTILS_DIRS): make_er7_makefiles icg_sim_serv
	@ $(MAKE) -C $@ trick

.PHONY: make_er7_makefiles
make_er7_makefiles:
	@for i in $(ER7_UTILS_DIRS) ; do \
	   $(CP) ${TRICK_HOME}/trick_source/sim_services/Executive/Makefile $$i; \
	done

ifeq ($(USE_ER7_UTILS_INTEGRATORS), 1)
icg_sim_serv: | make_er7_makefiles
endif

# 1.1.1.4 Generate interface code (using ICG) for the specified sim_services
# header files.
.PHONY: icg_sim_serv
icg_sim_serv: $(ICG_EXE)
	${TRICK_HOME}/bin/trick-ICG -s ${TRICK_CXXFLAGS} ${TRICK_HOME}/include/trick/files_to_ICG.hh

# 1.1.1.4.1 Build the Interface Code Generator (ICG) executable.
$(ICG_EXE) :
	$(MAKE) -C trick_source/codegen/Interface_Code_Gen

# 1.1.1.5 Create Trick Library directory.
$(TRICK_LIB_DIR):
	@ mkdir $@

# 1.1.2 Build libTrick_pyip.a (Swig Lib)
$(TRICK_SWIG_LIB): $(SWIG_DIRS) | $(TRICK_LIB_DIR)
	ar crs $@ $(SWIG_OBJS)

.PHONY: $(SWIG_DIRS)
$(SWIG_DIRS): icg_sim_serv $(TRICK_LIB_DIR)
	@ $(MAKE) -C $@ trick

#-------------------------------------------------------------------------------
# 1.2 Build Trick's Data-products Applications.
.PHONY: dp
dp: ${TRICK_HOME}/trick_source/trick_utils/units
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/data_products

#-------------------------------------------------------------------------------
# 1.3 Build Trick's Java Tools
java:
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/java

.PHONY: javadoc
javadoc:
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/java $@

#-------------------------------------------------------------------------------
# 1.4 This target builds the Trick Documentation.
.PHONY: doxygen
doxygen:
	@ $(MAKE) -C $@

#-------------------------------------------------------------------------------
# 1.5 Some Trick source is auto-generated as part of the Trick's build process. When
# Trick is distributed to the user community, we can't be certain that everyone's
# machine will have the approriate versions of the code generations tool. So rather
# than just hope, we go ahead and pre-generate the necessary source files, and
# include those in the distribution package.
# This target pre-generates these source files, that are necessary for creating
# a distribution package.
premade:
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/sim_services/MemoryManager premade
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/sim_services/CheckPointAgent premade
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/java

################################################################################
#                                   TESTING
################################################################################
# This target runs Trick's Unit-tests and simulation-tests.
test: unit_test sim_test
	@ echo "All tests completed sucessfully"

.PHONY: $(UNIT_TEST_DIRS)
$(UNIT_TEST_DIRS):
	@ $(MAKE) -C $@ test

unit_test: $(UNIT_TEST_DIRS)

sim_test:
	@ $(MAKE) -C trick_sims test

#requirements:
#	@ $(MAKE) -C trick_test/requirements_docs install

################################################################################
#                                 CLEAN Targets
################################################################################


clean: clean_sim_serv clean_utils clean_swig clean_dp clean_ICG clean_java
	@/bin/rm -rf $(TRICK_BIN_DIR)
	@/bin/rm -rf $(TRICK_LIB_DIR)

ifeq ($(USE_ER7_UTILS_INTEGRATORS), 1)
clean: clean_er7_utils 
endif

clean_sim_serv:
	@for i in $(SIM_SERV_DIRS) ; do \
	   $(MAKE) -C $$i real_clean ; \
	done
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/sim_services/mains real_clean

clean_er7_utils: make_er7_makefiles
	@for i in $(ER7_UTILS_DIRS) ; do \
	   $(MAKE) -C $$i real_clean ; \
	   rm $$i/Makefile; \
	done

clean_utils:
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/trick_utils real_clean

clean_swig:
	@for i in $(SWIG_DIRS) ; do \
	   $(MAKE) -C $$i real_clean ; \
	done

ifeq ($(USE_ER7_UTILS_INTEGRATORS), 1)
clean_swig: make_er7_makefiles 
endif

ifeq ($(DEVELOPER),1)
clean_ICG :
	$(MAKE) -C ${TRICK_HOME}/trick_source/codegen/Interface_Code_Gen  clean
else
clean_ICG :
endif

clean_unit_test:
	@/bin/rm -rf ${TRICK_HOME}/trick_test/*.xml
	@ for i in $(UNIT_TEST_DIRS) ; do \
	    $(MAKE) -C $$i clean ; \
	done

clean_doxygen:
	@ $(MAKE) -C ${TRICK_HOME}/doxygen clean


clean_dp:
	@ $(MAKE) clean -C ${TRICK_HOME}/trick_source/data_products

clean_java:
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/java clean


# FIXME: Seems to me that the for loop below should be removed and that the
#        'clean' target in trick_sims/makefile should be doing this. --Penn
clean_test: clean_unit_test
	-@ $(MAKE) -C trick_sims clean
	@for i in $(MODEL_DIRS) ; do \
	   cd $$i ; /bin/rm -rf io_src object_* swig xml ; \
	done

#clean_requirements:
#	@ $(MAKE) -C trick_test/requirements_docs clean

clean_stand_alone_utils:
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/trick_utils clean_stand_alone

clean_stand_alone: clean_stand_alone_utils

clean_objs_for_rpm: clean_sim_serv clean_er7_utils clean_utils
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/java clean_obj

clean_gui: clean_java

################################################################################
#                                 INSTALL Targets
################################################################################

install: copy_files

.PHONY: copy_files
copy_files: ${PREFIX}/trick/trick-$(TRICK_VERSION) copy_trick_source
	cp -r ${TRICK_HOME}/bin ${PREFIX}/trick/trick-$(TRICK_VERSION)
	cp ${TRICK_HOME}/config_Linux.mk ${PREFIX}/trick/trick-$(TRICK_VERSION)
	cp -r ${TRICK_HOME}/docs ${PREFIX}/trick/trick-$(TRICK_VERSION)
	cp -r ${TRICK_HOME}/lib_${TRICK_HOST_CPU} ${PREFIX}/trick/trick-$(TRICK_VERSION)
	cp -r ${TRICK_HOME}/makefiles ${PREFIX}/trick/trick-$(TRICK_VERSION)
	cp -r ${TRICK_HOME}/man ${PREFIX}/trick/trick-$(TRICK_VERSION)

${PREFIX}/trick/trick-$(TRICK_VERSION) :
	mkdir -p $@

###########

copy_trick_source: copy_codegen copy_sim_objects copy_sim_serv_dirs copy_utils_dirs copy_swig

ifeq ($(USE_ER7_UTILS_INTEGRATORS), 1)
copy_trick_source: copy_er7_utils_dirs
endif

${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source : ${PREFIX}/trick/trick-$(TRICK_VERSION)
	mkdir -p $@

###########

copy_codegen: ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/codegen/Interface_Code_Gen
	cp ${TRICK_HOME}/trick_source/codegen/Interface_Code_Gen/ICG_* ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/codegen/Interface_Code_Gen
	cp -r ${TRICK_HOME}/trick_source/codegen/Interface_Code_Gen/lib ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/codegen/Interface_Code_Gen

${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/codegen/Interface_Code_Gen : ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source
	mkdir -p $@

###########

copy_sim_objects: ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source
	cp -r ${TRICK_HOME}/trick_source/sim_objects ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source

###########

BARE_ER7_UTILS_DIRS = $(patsubst ${TRICK_HOME}/trick_source/er7_utils/%,%,$(ER7_UTILS_DIRS))
TARGET_ER7_UTILS_DIRS = $(addprefix ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/er7_utils/,$(BARE_ER7_UTILS_DIRS))

${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/er7_utils : ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source
	mkdir -p $@

$(TARGET_ER7_UTILS_DIRS): ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/er7_utils
	mkdir -p $@

copy_er7_utils_dirs: $(TARGET_ER7_UTILS_DIRS)
	@for i in $(BARE_ER7_UTILS_DIRS) ; do \
	   cp -r ${TRICK_HOME}/trick_source/er7_utils/$$i/include ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/er7_utils/$$i ; \
	done

###########

BARE_SIM_SERV_DIRS = $(filter-out include,$(notdir $(SIM_SERV_DIRS)))
BARE_SIM_SERV_DIRS += InputProcessor
TARGET_SIM_SERV_DIRS = $(addprefix ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/sim_services/,$(BARE_SIM_SERV_DIRS))
TARGET_SIM_SERV_DIRS += ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/sim_services/include

${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/sim_services : ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source
	mkdir -p $@

$(TARGET_SIM_SERV_DIRS): ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/sim_services
	mkdir -p $@

copy_sim_serv_dirs: $(TARGET_SIM_SERV_DIRS)
	cp ${TRICK_HOME}/trick_source/sim_services/include/*.h ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/sim_services/include
	cp ${TRICK_HOME}/trick_source/sim_services/include/*.hh ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/sim_services/include
	for i in $(BARE_SIM_SERV_DIRS) ; do \
	   cp -r ${TRICK_HOME}/trick_source/sim_services/$$i/include ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/sim_services/$$i ; \
	done

###########

BARE_UTILS_DIRS = $(notdir $(UTILS_DIRS))
TARGET_UTILS_DIRS = $(addprefix ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/trick_utils/,$(BARE_UTILS_DIRS))

${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/trick_utils : ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source
	mkdir -p $@

$(TARGET_UTILS_DIRS): ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/trick_utils
	mkdir -p $@

copy_utils_dirs: $(TARGET_UTILS_DIRS)
	for i in $(BARE_UTILS_DIRS) ; do \
	   cp -r ${TRICK_HOME}/trick_source/trick_utils/$$i/include ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/trick_utils/$$i ; \
	done

###########

TARGET_SWIG_DIRS = \
 ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/trick_swig \
 ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/trick_swig/swig_${TRICK_HOST_CPU}

$(TARGET_SWIG_DIRS): ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source
	mkdir -p $@

copy_swig: $(TARGET_SWIG_DIRS)
	cp ${TRICK_HOME}/trick_source/trick_swig/*.py ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/trick_swig
	cp ${TRICK_HOME}/trick_source/trick_swig/swig_${TRICK_HOST_CPU}/*.py ${PREFIX}/trick/trick-$(TRICK_VERSION)/trick_source/trick_swig/swig_${TRICK_HOST_CPU}

###########

uninstall:
	rm -rf ${PREFIX}/trick/trick-$(TRICK_VERSION)

###########

# These rules run the alternatives command in linux to create links in /usr/local/bin for Trick.
ifeq ($(TRICK_HOST_TYPE),Linux)
ALTERNATIVES := $(shell which alternatives || which update-alternatives)

install: set_alternatives
.PHONY: set_alternatives
set_alternatives: copy_files
	- ${ALTERNATIVES} --install /usr/local/bin/CP trick ${PREFIX}/trick/trick-$(TRICK_VERSION)/bin/CP 10 \
  --slave /usr/local/bin/ICG ICG /usr/local/trick/trick-$(TRICK_VERSION)/bin/ICG \
  --slave /usr/local/bin/gte gte /usr/local/trick/trick-$(TRICK_VERSION)/bin/gte \
  --slave /usr/local/bin/kill_sim kill_sim /usr/local/trick/trick-$(TRICK_VERSION)/bin/kill_sim \
  --slave /usr/local/bin/sie sie /usr/local/trick/trick-$(TRICK_VERSION)/bin/sie \
  --slave /usr/local/bin/sim_control sim_control /usr/local/trick/trick-$(TRICK_VERSION)/bin/sim_control \
  --slave /usr/local/bin/sniffer sniffer /usr/local/trick/trick-$(TRICK_VERSION)/bin/sniffer \
  --slave /usr/local/bin/trick_dp trick_dp /usr/local/trick/trick-$(TRICK_VERSION)/bin/trick_dp \
  --slave /usr/local/bin/trick_version trick_version /usr/local/trick/trick-$(TRICK_VERSION)/bin/trick_version \
  --slave /usr/local/bin/tv tv /usr/local/trick/trick-$(TRICK_VERSION)/bin/tv

uninstall: remove_alternatives
.PHONY: remove_alternatives
remove_alternatives:
	- ${ALTERNATIVES} --remove trick ${PREFIX}/trick/trick-$(TRICK_VERSION)/bin/CP
endif

################################################################################
#                    MISCELLANEOUS DEVELOPER UTILITY TARGETS                   #
################################################################################
stand_alone: stand_alone_utils
stand_alone_utils:
	@ $(MAKE) -C ${TRICK_HOME}/trick_source/trick_utils stand_alone

# The below rules are used by Debian based .deb file creating.

# ICG all sim_services files (for testing and debugging ICG).
# The -f flag forces io_src files to be regenerated whether or not they need to be.
ICG: $(ICG_EXE)
	${TRICK_HOME}/bin/ICG -f -s ${TRICK_CXXFLAGS} ${TRICK_HOME}/include/trick/files_to_ICG.hh

# This builds a tricklib share library.
ifeq ($(USE_ER7_UTILS_INTEGRATORS), 1)
trick_lib: $(SIM_SERV_DIRS) $(ER7_UTILS_DIRS) $(UTILS_DIRS) | $(TRICK_LIB_DIR)
	${TRICK_CPPC} $(SHARED_LIB_OPT) -o ${TRICK_LIB_DIR}/libtrick.so $(SIM_SERV_OBJS) $(ER7_UTILS_OBJS) $(UTILS_OBJS)
else
trick_lib: $(SIM_SERV_DIRS) $(UTILS_DIRS) | $(TRICK_LIB_DIR)
	${TRICK_CPPC} $(SHARED_LIB_OPT) -o ${TRICK_LIB_DIR}/libtrick.so $(SIM_SERV_OBJS) $(UTILS_OBJS)
endif

# For NASA/JSC developers include optional rules
-include Makefile_jsc_dirs

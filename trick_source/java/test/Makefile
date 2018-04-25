# To run new unit tests, add new tests to the "test" target

TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../../..)

# Get JAVAC definition
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

JAVAC ?= javac
JAVAC_FLAGS = -g -Xlint:unchecked -Xlint:deprecation
# delineate 'javac -version' for version number 
JAVAC_VERSION := $(shell ${JAVAC} -version 2>&1 | perl -ne 'print /(\d+)/')

# include java.se.ee for JAXB xml annotations (for trick-tv) in java version 9
ifeq ($(JAVAC_VERSION),9)
	JAVAC_FLAGS += --add-modules java.se.ee
endif
ifeq ($(JAVAC_VERSION),10)
	JAVAC_FLAGS += --add-modules java.se.ee
endif

SRC_DIR   = src
SRC_FILES = $(shell find ${SRC_DIR} -type f -name \*.java)
BUILD_DIR = build
DEST_DIR  = ${BUILD_DIR}/classes
LIB_DIR   = ${TRICK_HOME}/$(LIBEXEC)/trick/java/lib
DIST_DIR  = ${TRICK_HOME}/$(LIBEXEC)/trick/java/dist
TEST_DIR  = ${TRICK_HOME}/$(LIBEXEC)/trick/java/test
TRICK_TEST = ${TRICK_HOME}/trick_test

empty :=
space := $(empty) $(empty)
CLASS_PATH = $(subst $(space),:,$(wildcard ${LIB_DIR}/*.jar):${DIST_DIR}/trick.jar)

RESOURCES = $(subst src/,,$(shell find src/trick -name resources))
RESOURCES += $(subst src/,,$(shell find src/trick -name jaxb.index))

all: test
	@echo "[32mJava test successful[0m"
test: ${TEST_DIR}/test.jar ${TRICK_TEST}
	@echo "[36mtrick.common.RunTimeTrickApplicationTest[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_RunTimeTrickApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.common.RunTimeTrickApplicationTest
	@echo "[36mtrick.common.TrickApplicationTest[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_TrickApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.common.TrickApplicationTest
	@echo "[36mtrick.common.utils.LogHeaderReaderTest[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_LogHeaderReaderTest.xml barrypitman.junitXmlFormatter.Runner trick.common.utils.LogHeaderReaderTest
	@# Error with UnitTypeTest
	@# java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_UnitTypeTest.xml barrypitman.junitXmlFormatter.Runner trick.common.utils.UnitTypeTest
	@echo "[36mtrick.dataproducts.trickdp.TrickDPApplicationTest[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_TrickDPApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.dataproducts.trickdp.TrickDPApplicationTest
	@echo "[36mtrick.dataproducts.trickqp.TrickQPApplicationTest[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_TrickQPApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.dataproducts.trickqp.TrickQPApplicationTest
	@echo "[36mtrick.dre.DreApplicationTest[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_DreApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.dre.DreApplicationTest
	@echo "[36mtrick.montemonitor.MonteMonitorApplicationTest[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_MonteMonitorApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.montemonitor.MonteMonitorApplicationTest
	@echo "[36mtrick.mtv.MtvApplicationTest[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_MtvApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.mtv.MtvApplicationTest
	@# SieApplicationTest needs to be updated
	@# java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_SieApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.sie.SieApplicationTest
	@echo "[36mtrick.sniffer.SnifferApplicationTest[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_SnifferApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.sniffer.SnifferApplicationTest
	@echo "[36mtrick.tv.TrickApplicationTest[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_TVApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.tv.TVApplicationTest
	@echo "[36mtrick.simcontrol.SimControlApplicationTest[0m"
	@echo "[33mCLICK OK ON GUI TO CONTINUE[0m"
	@java -cp "${LIB_DIR}/*:${DIST_DIR}/*:${TEST_DIR}/*" -Dorg.schmant.task.junit4.target=${TRICK_HOME}/trick_test/Java_SimControlApplicationTest.xml barrypitman.junitXmlFormatter.Runner trick.simcontrol.SimControlApplicationTest
	
${DEST_DIR} ${TEST_DIR} ${TRICK_TEST}:
	mkdir -p $@

${TEST_DIR}/test.jar: compile ${TEST_DIR}
	@echo "[36mCreating jar file...[0m"
	@jar cf ${TEST_DIR}/test.jar -C ${DEST_DIR} .

trick_guis:
	@ $(MAKE) -C  ..

compile: ${DEST_DIR} trick_guis
	@echo "[36mBuilding Trick GUI tests ..[0m"
	@${JAVAC} ${JAVAC_FLAGS} -classpath ${CLASS_PATH} -d ${DEST_DIR} -sourcepath ${SRC_DIR} ${SRC_FILES}

clean:
	rm -rf ${BUILD_DIR}
	rm -rf ${TEST_DIR}

clean_obj:
	rm -rf ${BUILD_DIR}

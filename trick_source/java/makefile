TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../..)

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
RESOURCES_DIR  = ${TRICK_HOME}/$(LIBEXEC)/trick/java/resources
DOCS_API_DIR   = ${TRICK_HOME}/share/doc/trick/java
TEST_REPORTS_DIR = test/reports

empty :=
space := $(empty) $(empty)
CLASS_PATH = $(subst $(space),:,$(wildcard ${LIB_DIR}/*.jar))

RESOURCES = $(subst src/,,$(shell find src/trick -name resources))
RESOURCES += $(subst src/,,$(shell find src/trick -name jaxb.index))

all: ${DIST_DIR}/trick.jar resources
	@echo "[32mJava build successful[0m"

${DEST_DIR} ${DIST_DIR} ${RESOURCES_DIR} ${DOCS_API_DIR}:
	mkdir -p $@

${DIST_DIR}/trick.jar: compile ${DIST_DIR}
	@echo "[36mCreating jar file...[0m"
	@jar cf ${DIST_DIR}/trick.jar -C ${DEST_DIR} .
	@cd ${SRC_DIR} && jar uf ${DIST_DIR}/trick.jar ${RESOURCES}

external_jars:
	@ $(MAKE) -C  $(LIB_DIR)

compile: ${DEST_DIR} external_jars
	@echo "[36mBuilding Trick GUIs...[0m"
	@${JAVAC} ${JAVAC_FLAGS} -classpath ${CLASS_PATH} -d ${DEST_DIR} -sourcepath ${SRC_DIR} ${SRC_FILES}

resources: ${RESOURCES_DIR}/trick_icon.png

${RESOURCES_DIR}/trick_icon.png: ${SRC_DIR}/trick/common/resources/trick_icon.png ${RESOURCES_DIR}
	@cp $< $@

JAVA_DOC_LINKS = \
 -link "http://docs.oracle.com/javase/6/docs/api/" \
 -link "http://www.jfree.org/jfreechart/api/javadoc/" \
 -link "http://junit.sourceforge.net/javadoc/"

javadoc: ${DOCS_API_DIR}
	javadoc -d ${DOCS_API_DIR} -classpath ${CLASS_PATH} -sourcepath ${SRC_DIR} -windowtitle "Trick GUI API" ${JAVA_DOC_LINKS} trick ${SRC_FILES}

clean:
	rm -rf ${BUILD_DIR}
	rm -rf ${DIST_DIR}
	rm -rf ${RESOURCES_DIR}
	rm -rf ${TEST_REPORTS_DIR}

clean_obj:
	rm -rf ${BUILD_DIR}

clean_docs:
	rm -rf ${DOCS_API_DIR}

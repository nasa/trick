TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../..)
JAVA_BUILD_DIR = ${TRICK_HOME}/libexec/trick/java/build


include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

all:
	@echo "[32mBuilding java application...[0m"
	@${MVN} -q package

javadoc:
	${MVN} javadoc:javadoc

clean:
	@${MVN} -q clean
	@echo java build directory cleaned.

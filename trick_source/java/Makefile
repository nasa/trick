TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../..)
JAVA_BUILD_DIR = ${TRICK_HOME}/libexec/trick/java/build


include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

all:
	@echo "[32mBuilding java application...[0m"
	@${MVN} package -q -Dcmake=false

javadoc:
	${MVN} javadoc:javadoc

clean:
	@${MVN} -q clean -Dcmake=false
	@echo java build directory cleaned.

TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../..)
JAVA_BUILD_DIR = ${TRICK_HOME}/libexec/trick/java/build


include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

all:
	@echo "[32mBuilding java application...[0m"
	@${MVN} package -q -Dcmake=false

javadoc:
	${MVN} javadoc:javadoc

clean:
	rm -rf ${JAVA_BUILD_DIR}
	@echo java build directory cleaned.

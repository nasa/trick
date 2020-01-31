TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../..)
JAVA_BUILD_DIR = ${TRICK_HOME}/libexec/trick/java/build
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

all: ${TRICK_HOME}/libexec/trick/java/build
	@echo "[32mJava build successful[0m"

${TRICK_HOME}/libexec/trick/java/build:
	@echo "[32mBuilding java application... For details see ${JAVA_BUILD_DIR}/maven.out[0m"
	@mkdir ${JAVA_BUILD_DIR}
	@${MVN} package &> ${JAVA_BUILD_DIR}/maven.out

javadoc:
	${MVN} javadoc:javadoc

clean:
	@rm -rf ${JAVA_BUILD_DIR}
	@echo java build directory cleaned.



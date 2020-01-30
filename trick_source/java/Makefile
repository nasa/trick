TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../..)

all: ${TRICK_HOME}/libexec/trick/java/build
	@echo "[32mJava build successful[0m"

${TRICK_HOME}/libexec/trick/java/build:
	mvn package 

javadoc:
	mvn javadoc:javadoc

clean:
	mvn clean

TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../..)

all: ${TRICK_HOME}/libexec/trick/java/build resources
	@echo "[32mJava build successful[0m"

${TRICK_HOME}/libexec/trick/java/build:
	mvn package 

javadoc:
	mvn javadoc:javadoc

clean:
	mvn clean

resources: ${RESOURCES_DIR}/trick_icon.png: ${SRC_DIR}/trick/common/resources/trick_icon.png ${RESOURCES_DIR}
	@cp $< $@
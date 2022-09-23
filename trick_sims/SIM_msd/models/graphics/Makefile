SHELL = /bin/sh

PROJECT_NAME = MsdGui
SRC_DIR = MSD/src
BUILD_DIR = build
CLASSES_DIR = $(BUILD_DIR)/classes
JAR_DIR = dist
MAIN_CLASS = MsdGui

all: jar

clean:
	rm -rf $(BUILD_DIR)
	rm -f manifest

spotless: clean
	rm -rf dist

$(CLASSES_DIR):
	@ mkdir -p $(CLASSES_DIR)

compile: | $(CLASSES_DIR)
	javac -sourcepath $(SRC_DIR) -d $(CLASSES_DIR) $(SRC_DIR)/MsdGui.java

manifest:
	@ echo "Main-Class: $(MAIN_CLASS)" > $@

$(JAR_DIR):
	@ mkdir -p $(JAR_DIR)

jar: compile manifest | $(JAR_DIR)
	jar cvfm $(JAR_DIR)/$(PROJECT_NAME).jar manifest -C $(CLASSES_DIR) .
	@ echo "-------------------------------------------------------------------------------"
	@ echo "                              BUILD COMPLETE"
	@ echo "The Java jar file (the Java Executable) is located at: $(JAR_DIR)/$(PROJECT_NAME).jar"
	@ echo "-------------------------------------------------------------------------------"

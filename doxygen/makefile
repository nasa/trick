
export TRICK_DOCS = $(TRICK_HOME)/share/doc/trick
export TRICK_RELEASE = $(shell $(TRICK_HOME)/bin/trick-version -v)
export TRICK_MAJOR = $(shell echo $(TRICK_RELEASE) | cut -f 1 -d.)
export TRICK_MINOR = $(shell echo $(TRICK_RELEASE) | cut -f 2 -d.)

all:
	doxygen Doxyfile
	@ cp ${TRICK_HOME}/trick_source/java/src/trick/common/resources/trick_small.gif  ${TRICK_DOCS}/html
	@ cp ${TRICK_HOME}/doxygen/trick_icon.png  ${TRICK_DOCS}/html

clean:
	@ test `uname -s` = Darwin \
    && \
        echo "Removing generated Doxygen files... \c" \
    || \
        echo -n "Removing generated Doxygen files... "
	@ rm -rf ${TRICK_DOCS}/html
	@ rm -rf ${TRICK_DOCS}/latex
	@ rm -rf ${TRICK_DOCS}/rtf
	@ rm -rf ${TRICK_DOCS}/trick.tag
	@ rm -rf ${TRICK_DOCS}/doxygen_*.tmp
	@ rm -rf design/*.dox
	@ rm -rf requirements/*.dox
	@ rm -rf users_guide/*.dox
	@ rm -rf warning_msgs
	@ echo "complete."

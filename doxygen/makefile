# $Id: makefile 3443 2014-02-21 14:49:26Z alin $

export TRICK_DOCS = $(TRICK_HOME)/docs
export TRICK_RELEASE = $(shell $(TRICK_HOME)/bin/trick_version -v)
export TRICK_MAJOR = $(shell echo $(TRICK_RELEASE) | cut -f 1 -d.)
export TRICK_MINOR = $(shell echo $(TRICK_RELEASE) | cut -f 2 -d.)

all:
	cd users_guide ; ../auto_number.pl u
	cd requirements ; ../auto_number.pl r
	cd design ; ../auto_number.pl d
	doxygen Doxyfile
	@ cp */*.html  ${TRICK_DOCS}/html
	@ cp ${TRICK_HOME}/trick_source/java/src/trick/common/resources/trick_small.gif  ${TRICK_DOCS}/html
	@ cp ${TRICK_HOME}/doxygen/trick_icon.png  ${TRICK_DOCS}/html

rtf_users_guide:
	cd users_guide ; ../auto_number.pl u
	doxygen Doxyfile_rtf_users_guide

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

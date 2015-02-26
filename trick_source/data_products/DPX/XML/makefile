
# $Id: makefile 639 2010-04-14 15:54:53Z wwoodard $

PREPROCESSOR = /usr/bin/m4
RM = rm -rf

all: catalog.xml
catalog.xml: catalog.xml.template
	${PREPROCESSOR} --define=TRICK_HOME=${TRICK_HOME}   $< > $@

clean:
	${RM} catalog.xml

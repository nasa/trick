SHELL = /bin/sh

ifndef TRICK_HOST_CPU
TRICK_HOST_CPU := $(shell trick-gte TRICK_HOST_CPU)
endif

ifndef TRICK_HOST_TYPE
TRICK_HOST_TYPE := $(shell trick-gte TRICK_HOST_TYPE)
endif


CC  = gcc
CPP = g++

CFLAGS = -g -Wall -I../include 

RM = /bin/rm -rf

LIBDIR = ${TRICK_HOME}/lib_${TRICK_HOST_CPU}
LIBS = -ltrick_units -lm

INCLUDES = -I.. 

.c.o:
	${CC} ${CFLAGS} ${INCLUDES} -c $<

.cpp.o:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<

top:
	@ if [ "${TRICK_HOME}" != "" ] ; then  \
	      $(MAKE) -e all                   \
	      "CC =  `trick-gte TRICK_CC` "          \
	      "CPP = `trick-gte TRICK_CXX` ";       \
	else                                   \
 	      $(MAKE) all                      \
	      "TRICK_HOST_TYPE = `uname -s` "  \
	      "TRICK_HOST_CPU =  `uname -s` "; \
	fi

all: units_conv_c_test unit_cpp_test

units_conv_c_test : units_conv_c_test.o
	${CC} -o $@ units_conv_c_test.o -L${LIBDIR} ${LIBS}

unit_cpp_test: unit_cpp_test.o
	${CPP} -o $@ unit_cpp_test.o -L${LIBDIR} ${LIBS}

clean:
	${RM} *.o
	${RM} *~
	${RM} *.exe

real_clean: clean
	${RM} units_conv_c_test
	${RM} unit_cpp_test

UCFn.o: UCFn.hh
Unit.o: Unit.hh units_conv.h
units_conv.o: units_conv.h

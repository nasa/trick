SHELL = /bin/sh

ifndef TRICK_HOST_CPU
TRICK_HOST_CPU := $(shell ${TRICK_HOME}/bin/trick-gte TRICK_HOST_CPU)
endif

ifndef TRICK_HOST_TYPE
TRICK_HOST_TYPE := $(shell ${TRICK_HOME}/bin/trick-gte TRICK_HOST_TYPE)
endif


CPP = g++

CFLAGS = -g -Wall -I${TRICK_HOME}/include

RM = /bin/rm -rf

LIBDIR = ${TRICK_HOME}/lib64
LIBS = -ltrick_math -lm

INCLUDES = -I${TRICK_HOME}/include 

.cpp.o:
	${CPP} ${CFLAGS} ${INCLUDES} -c $<

top:
	@ if [ "${TRICK_HOME}" != "" ] ; then  \
	      $(MAKE) -e all                   \
	      "CPP = `${TRICK_HOME}/bin/trick-gte TRICK_CPPC` ";       \
	else                                   \
 	      $(MAKE) all                      \
	      "TRICK_HOST_TYPE = `uname -s` "  \
	      "TRICK_HOST_CPU =  `uname -s` "; \
	fi

all:  euler_quat_test

euler_quat_test: euler_quat_test.o
	${CPP} -o $@ euler_quat_test.o -L${LIBDIR} ${LIBS}

clean:
	${RM} *.o
	${RM} *~
	${RM} *.exe
	${RM} euler_quat_test

real_clean: clean
	${RM} euler_quat_test

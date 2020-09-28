
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf
CC = cc
CPP = c++

CFLAGS = -g -Wall
INCLUDE_DIRS = -I../include -I$(GTEST_HOME)/include

SAI_OBJDIR = obj
SAI_LIBDIR = ../lib
SAI_LIBNAME = libSAInteg.a
SAI_LIBOBJS = ${SAI_OBJDIR}/Integrator.o

LIBDIRS = -L${SAI_LIBDIR} -L${GTEST_HOME}/lib64 -L${GTEST_HOME}/lib

all: test 

test: SAIntegrator_unittest
	./SAIntegrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/SAIntegrator_unittest.xml

SAIntegrator_unittest.o : SAIntegrator_unittest.cc
	$(CPP) $(CFLAGS) $(INCLUDE_DIRS) -c $<

SAIntegrator_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} SAIntegrator_unittest.o
	$(CPP) $(CFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread

${SAI_LIBDIR}/${SAI_LIBNAME} :
	$(MAKE) -C ..

clean:
	${RM} *.o

spotless: clean
	${RM} SAIntegrator_unittest

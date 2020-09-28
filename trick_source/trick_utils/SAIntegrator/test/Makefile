
RM = rm -rf
CC = cc
CPP = c++

CFLAGS = -g -Wall
INCLUDE_DIRS = -I../include
OBJDIR = obj
LIBDIR = ../lib
LIBNAME = libSAInteg.a
LIBOBJS = ${OBJDIR}/Integrator.o

all: SAIntegrator_unittest 

test: SAIntegrator_unittest
        ./SAIntegrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/SAIntegrator_unittest.xml


Integ_unittest.o : Integ_unittest.cc
	$(CPP) $(CFLAGS) $(INCLUDE_DIRS) -c $<

Integ_unittest : ${LIBDIR}/${LIBNAME} Integ_unittest.o
	$(CPP) $(CFLAGS) -o $@ $^ -lgtest -lgtest_main -lpthread

${LIBDIR}/${LIBNAME} :
	$(MAKE) -C ..

clean:
	${RM} *.o

spotless: clean
	${RM} Integ_unittest

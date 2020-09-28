
RM = rm -rf
CC = cc
CPP = c++

CFLAGS = -g -Wall
INCLUDE_DIRS = -I../include
OBJDIR = obj
LIBDIR = ../lib
LIBNAME = libSAInteg.a
LIBOBJS = ${OBJDIR}/Integrator.o

all: test 

test: SAIntegrator_unittest
	./SAIntegrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/SAIntegrator_unittest.xml

SAIntegrator_unittest.o : SAIntegrator_unittest.cc
	$(CPP) $(CFLAGS) $(INCLUDE_DIRS) -c $<

SAIntegrator_unittest : ${LIBDIR}/${LIBNAME} SAIntegrator_unittest.o
	$(CPP) $(CFLAGS) -o $@ $^ -L${LIBDIR} -lSAInteg -lgtest -lgtest_main -lpthread

${LIBDIR}/${LIBNAME} :
	$(MAKE) -C ..

clean:
	${RM} *.o

spotless: clean
	${RM} SAIntegrator_unittest


include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf

TRICK_CPPFLAGS += -I$(GTEST_HOME)/include -I$(TRICK_HOME)/include -g -Wall -Wextra -DGTEST_HAS_TR1_TUPLE=0 ${TRICK_SYSTEM_CXXFLAGS}
INCLUDE_DIRS = -I../include -I$(GTEST_HOME)/include

SAI_OBJDIR = obj
SAI_LIBDIR = ../lib
SAI_LIBNAME = libSAInteg.a
SAI_LIBOBJS = ${SAI_OBJDIR}/Integrator.o

LIBDIRS = -L${SAI_LIBDIR} -L${GTEST_HOME}/lib64 -L${GTEST_HOME}/lib

all: test

test: SAIntegrator_unittest RootFinder_unittest
	./SAIntegrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/SAIntegrator_unittest.xml
	./RootFinder_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/RootFinder_unittest.xml

SAIntegrator_unittest.o : SAIntegrator_unittest.cc
	$(TRICK_CXX) $(TRICK_CPPFLAGS) $(INCLUDE_DIRS) -c $<

SAIntegrator_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} SAIntegrator_unittest.o
	$(TRICK_CXX) $(TRICK_CPPFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread

RootFinder_unittest.o : RootFinder_unittest.cc
	$(TRICK_CXX) $(TRICK_CPPFLAGS) $(INCLUDE_DIRS) -c $<

RootFinder_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} RootFinder_unittest.o
	$(TRICK_CXX) $(TRICK_CPPFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread

${SAI_LIBDIR}/${SAI_LIBNAME} :
	$(MAKE) -C ..

clean:
	${RM} *.o

spotless: clean
	${RM} SAIntegrator_unittest

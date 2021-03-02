
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf

TRICK_CPPFLAGS += -I$(GTEST_HOME)/include -I$(TRICK_HOME)/include -g -Wall -Wextra -DGTEST_HAS_TR1_TUPLE=0 ${TRICK_SYSTEM_CXXFLAGS}
INCLUDE_DIRS = -I../include -I$(GTEST_HOME)/include

SAI_OBJDIR = obj
SAI_LIBDIR = ../lib
SAI_LIBNAME = libSAInteg.a
SAI_LIBOBJS = ${SAI_OBJDIR}/Integrator.o

LIBDIRS = -L${SAI_LIBDIR} -L${GTEST_HOME}/lib64 -L${GTEST_HOME}/lib

TEST_PROGRAMS = SAIntegrator_unittest\
	            FirstOrderODEIntegrator_unittest\
	            FirstOrderODEVariableStepIntegrator_unittest\
	            EulerIntegrator_unittest\
				RK2Integrator_unittest\
				RK4Integrator_unittest\
				RK3_8Integrator_unittest\
				RKF45Integrator_unittest\
	            RootFinder_unittest

all: test

test: $(TEST_PROGRAMS)
	./SAIntegrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/SAIntegrator_unittest.xml
	./FirstOrderODEIntegrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/FirstOrderODEIntegrator_unittest.xml
	./FirstOrderODEVariableStepIntegrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/FirstOrderODEVariableStepIntegrator_unittest.xml
	./EulerIntegrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/EulerIntegrator_unittest.xml
	./RK2Integrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/RK2Integrator_unittest.xml
	./RK4Integrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/RK4Integrator_unittest.xml
	./RK3_8Integrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/RK3_8Integrator_unittest.xml
	./RKF45Integrator_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/RKF45Integrator_unittest.xml
	./RootFinder_unittest --gtest_output=xml:${TRICK_HOME}/trick_test/RootFinder_unittest.xml

SAIntegrator_unittest.o : SAIntegrator_unittest.cc
	$(TRICK_CXX) $(TRICK_CPPFLAGS) $(INCLUDE_DIRS) -c $<

SAIntegrator_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} SAIntegrator_unittest.o
	$(TRICK_CXX) $(TRICK_CPPFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread

FirstOrderODEIntegrator_unittest.o : FirstOrderODEIntegrator_unittest.cc
	$(TRICK_CXX) $(TRICK_CPPFLAGS) $(INCLUDE_DIRS) -c $<

FirstOrderODEIntegrator_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} FirstOrderODEIntegrator_unittest.o
	$(TRICK_CXX) $(TRICK_CPPFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread

FirstOrderODEVariableStepIntegrator_unittest.o : FirstOrderODEVariableStepIntegrator_unittest.cc
	$(TRICK_CXX) $(TRICK_CPPFLAGS) $(INCLUDE_DIRS) -c $<

FirstOrderODEVariableStepIntegrator_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} FirstOrderODEVariableStepIntegrator_unittest.o
	$(TRICK_CXX) $(TRICK_CPPFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread
# ====
EulerIntegrator_unittest.o : EulerIntegrator_unittest.cc
	$(TRICK_CXX) $(TRICK_CPPFLAGS) $(INCLUDE_DIRS) -c $<

EulerIntegrator_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} EulerIntegrator_unittest.o
	$(TRICK_CXX) $(TRICK_CPPFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread
# ====
RK2Integrator_unittest.o : RK2Integrator_unittest.cc
	$(TRICK_CXX) $(TRICK_CPPFLAGS) $(INCLUDE_DIRS) -c $<

RK2Integrator_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} RK2Integrator_unittest.o
	$(TRICK_CXX) $(TRICK_CPPFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread
# ====
RK4Integrator_unittest.o : RK4Integrator_unittest.cc
	$(TRICK_CXX) $(TRICK_CPPFLAGS) $(INCLUDE_DIRS) -c $<

RK4Integrator_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} RK4Integrator_unittest.o
	$(TRICK_CXX) $(TRICK_CPPFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread
# ====
RK3_8Integrator_unittest.o : RK3_8Integrator_unittest.cc
	$(TRICK_CXX) $(TRICK_CPPFLAGS) $(INCLUDE_DIRS) -c $<

RK3_8Integrator_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} RK3_8Integrator_unittest.o
	$(TRICK_CXX) $(TRICK_CPPFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread
# ====
RKF45Integrator_unittest.o : RKF45Integrator_unittest.cc
	$(TRICK_CXX) $(TRICK_CPPFLAGS) $(INCLUDE_DIRS) -c $<

RKF45Integrator_unittest : ${SAI_LIBDIR}/${SAI_LIBNAME} RKF45Integrator_unittest.o
	$(TRICK_CXX) $(TRICK_CPPFLAGS) -o $@ $^ ${LIBDIRS} -lSAInteg -lgtest -lgtest_main -lpthread
# ====
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
	${RM} FirstOrderODEIntegrator_unittest
	${RM} FirstOrderODEVariableStepIntegrator_unittest
	${RM} EulerIntegrator_unittest
	${RM} RK2Integrator_unittest
	${RM} RK4Integrator_unittest
	${RM} RK3_8Integrator_unittest
	${RM} RKF45Integrator_unittest
	${RM} RootFinder_unittest

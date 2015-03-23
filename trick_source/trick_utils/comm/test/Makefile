
include ${TRICK_HOME}/makefiles/Makefile.common
include ${TRICK_HOME}/makefiles/Makefile.tricklib

INCLUDE_DIRS   = -I$(GTEST_HOME) -I$(GTEST_HOME)/include -I$(TRICK_HOME)/trick_source

TRICK_LIBS     := ${TRICK_HOME}/lib_${TRICK_HOST_CPU}/libtrick.a
TRICK_EXEC_LINK_LIBS += ${GTEST_HOME}/lib/libgtest.a ${GTEST_HOME}/lib/libgtest_main.a

TRICK_CXXFLAGS += ${INCLUDE_DIRS} -g -Wall -Wextra -DGTEST_HAS_TR1_TUPLE=0

# Automatically determine all executable names produced by this Makefile.
TESTS = comm_test
TEST_CPP =  $(subst $(OBJ_DIR)/,,$(subst .o,.cpp,$(CPP_OBJS)))

OTHER_OBJECTS = ../../../sim_services/SimObject/object_${TRICK_HOST_CPU}/io_SimObject.o

default : all

all : objects $(TESTS)

# Set XML test results name
test : all
	@ for i in $(TESTS) ; do \
         ./$$i --gtest_output=xml:${TRICK_HOME}/trick_test/TrickComm.xml; \
      done

clean : clean_test

clean_test :
	$(RM) -rf $(TESTS)

$(TESTS) : $(CPP_OBJS)
	$(TRICK_LD) $(TRICK_CXXFLAGS) -o $@ $^ $(OTHER_OBJECTS) $(TRICK_LIBS) $(TRICK_EXEC_LINK_LIBS)


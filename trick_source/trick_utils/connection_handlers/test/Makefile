
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common
include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib

INCLUDE_DIRS   = -I$(GTEST_HOME) -I${GTEST_HOME}/include -I$(TRICK_HOME)/include

# Use the libtrick_connection_handlers library only.  libtrick.a would also work.
TRICK_LIBS     := ${TRICK_LIB_DIR}/libtrick_connection_handlers.a
TRICK_EXEC_LINK_LIBS += -L${GTEST_HOME}/lib64 -L${GTEST_HOME}/lib -lgtest -lgtest_main
TRICK_CXXFLAGS += ${INCLUDE_DIRS} -g -Wall -Wextra ${TRICK_SYSTEM_CXXFLAGS} ${TRICK_TEST_FLAGS}

# Automatically determine all executable names produced by this Makefile.
TESTS = comm_test

# Objects that we depend on
DEPENDS = ../object_*/*.o

TRICK_CXXFLAGS += -Wno-unused-parameter

default : all

all : $(TESTS)

# Set XML test results name
test :  all 
	@ for i in $(TESTS) ; do \
         ./$$i --gtest_output=xml:${TRICK_HOME}/trick_test/ConnectionHandlers.xml; \
      done

clean : clean_test

clean_test :
	$(RM) -rf $(TESTS)


$(TESTS) : $(CPP_OBJS) $(DEPENDS) 
	$(TRICK_CXX) $(TRICK_CXXFLAGS)  $^  $(TRICK_EXEC_LINK_LIBS) -o $@ $(SYSCALL_MOCK_LINKFLAGS)


include $(dir $(lastword $(MAKEFILE_LIST)))../../../../share/trick/makefiles/Makefile.common

TRICK_CPPFLAGS += -I$(GTEST_HOME)/include -I$(TRICK_HOME)/include -g ${TRICK_SYSTEM_CXXFLAGS} ${TRICK_TEST_FLAGS}
LIBS = -L${GTEST_HOME}/lib64 -L${GTEST_HOME}/lib -lgtest -lgtest_main -lpthread

# Build the production sources directly, using Executive/message stubs in the test.
SOURCES = RealtimeSync_test.cpp ../RealtimeSync.cpp ../../Clock/Clock.cpp ../../Timer/Timer.cpp
TESTS = RealtimeSync_test

.PHONY: all test clean
all: $(TESTS)

test: $(TESTS)
	./RealtimeSync_test --gtest_output=xml:${TRICK_HOME}/trick_test/RealtimeSync.xml

RealtimeSync_test: $(SOURCES)
	$(TRICK_CXX) $(TRICK_SYSTEM_LDFLAGS) $(TRICK_CPPFLAGS) -o $@ $^ $(LIBS)

clean:
	rm -f $(TESTS) *.o

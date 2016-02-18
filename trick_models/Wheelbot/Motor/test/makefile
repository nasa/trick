
RM = rm -rf
CC = cc
CPP = c++

DECL_DIR = ..
GTEST_DIR = ${HOME}/gtest-1.7.0

CFLAGS += -g -Wall -Wextra -I$(GTEST_DIR)/include -I$(DECL_DIR)/include

LIBS = ../lib/libMotor.a   -lpthread

TESTS = DCMotorTest PWMTest 

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

all : $(TESTS)

test: $(TESTS)

	./DCMotorTest              --gtest_output=xml:XMLtestReports/DCMotorTestResults.xml
	./PWMTest              --gtest_output=xml:XMLtestReports/PWMTestResults.xml
	
clean :
	rm -f $(TESTS) gtest.a gtest_main.a
	rm -f *.o
	rm -f *.cpp~
	rm -f *.hh~
	rm -rf XMLtestReports

gtest-all.o :
	$(CPP) -I$(GTEST_DIR) $(CFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o :
	$(CPP) -I$(GTEST_DIR) $(CFLAGS) -c $(GTEST_DIR)/src/gtest_main.cc

DCMotorTest.o : DCMotorTest.cpp
	$(CPP) $(CFLAGS) -c $<

DCMotorTest : DCMotorTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

PWMTest.o : PWMTest.cpp
	$(CPP) $(CFLAGS) -c $<

PWMTest : PWMTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

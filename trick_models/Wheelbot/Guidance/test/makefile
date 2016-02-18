

RM = rm -rf
CC = cc
CPP = c++

DECL_DIR = ..
GTEST_DIR = ${HOME}/gtest-1.7.0

CFLAGS += -g -Wall -Wextra -I$(GTEST_DIR)/include -I$(DECL_DIR)/include

LIBS = ../lib/libGuidance.a -lpthread

TESTS = ArenaTest FindPathTest NavigatorTest

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

all : $(TESTS)

test: $(TESTS)
	./ArenaTest              --gtest_output=xml:XMLtestReports/ArenaTestResults.xml
	./FindPathTest           --gtest_output=xml:XMLtestReports/FindPathTestResults.xml
	./NavigatorTest		 --gtest_output=xml:XMLtestReports/NavigatorTestResults.xml

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

ArenaTest.o : ArenaTest.cpp
	$(CPP) $(CFLAGS) -c $<

ArenaTest : ArenaTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

FindPathTest.o : FindPathTest.cpp
	$(CPP) $(CFLAGS) -c $<

FindPathTest : FindPathTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

NavigatorTest.o : NavigatorTest.cpp
	$(CPP) $(CFLAGS) -c $<

NavigatorTest : NavigatorTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) -o $@ $^ $(LIBS)

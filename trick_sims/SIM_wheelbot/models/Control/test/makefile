RM = rm -rf
CC = cc
CPP = c++

PROJECT_DIR = ../..

GTEST_DIR = ${HOME}/gtest-1.7.0

CFLAGS += -g -Wall -Wextra

INCLUDE_DIRS += -I$(PROJECT_DIR)\
	        -I$(GTEST_DIR)/include

LIBS = $(PROJECT_DIR)/Control/lib/libControl.a \
       $(PROJECT_DIR)/Guidance/lib/libGuidance.a \
       -lpthread

TESTS = DifferentialDriveControllerTest VehicleControllerTest

GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

all : $(TESTS)

test: $(TESTS)
	./DifferentialDriveControllerTest --gtest_output=xml:XMLtestReports/DifferentialDriveControllerTestResults.xml
	./VehicleControllerTest --gtest_output=xml:XMLtestReports/VehicleControllerTestResults.xml

clean :
	rm -f $(TESTS) gtest.a gtest_main.a
	rm -f *.o
	rm -f *.cpp~
	rm -f *.hh~
	rm -rf XMLtestReports

gtest-all.o :
	$(CPP) -I$(GTEST_DIR) -I$(GTEST_DIR)/include $(CFLAGS) -c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o :
	$(CPP) -I$(GTEST_DIR) -I$(GTEST_DIR)/include $(CFLAGS) -c $(GTEST_DIR)/src/gtest_main.cc

DifferentialDriveControllerTest.o : DifferentialDriveControllerTest.cpp
	$(CPP) $(CFLAGS) $(INCLUDE_DIRS) -c $<

DifferentialDriveControllerTest : DifferentialDriveControllerTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $^ $(LIBS)

VehicleControllerTest.o : VehicleControllerTest.cpp
	$(CPP) $(CFLAGS) $(INCLUDE_DIRS) -c $<

VehicleControllerTest : VehicleControllerTest.o gtest_main.o gtest-all.o
	$(CPP) $(CFLAGS) $(INCLUDE_DIRS) -o $@ $^ $(LIBS)

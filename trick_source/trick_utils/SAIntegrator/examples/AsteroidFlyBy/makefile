
RM = rm -rf
CC = cc
CPP = c++

CXXFLAGS = -g -Wall
INCLUDE_DIRS = -I../../include
LIBDIR = ../../lib

all: Flyby

Flyby: Flyby.cpp
	$(CPP) $(CXXFLAGS) Flyby.cpp ${INCLUDE_DIRS} -L${LIBDIR} -lSAInteg -o Flyby

clean:
	${RM} Flyby.dSYM

spotless: clean
	${RM} Flyby
	${RM} flyby.csv

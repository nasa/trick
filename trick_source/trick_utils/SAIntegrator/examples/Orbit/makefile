
RM = rm -rf
CC = cc
CPP = c++

CXXFLAGS = -g -Wall
INCLUDE_DIRS = -I../../include
LIBDIR = ../../lib

all: Orbit

Orbit: Orbit.cpp
	$(CPP) $(CXXFLAGS) Orbit.cpp ${INCLUDE_DIRS} -L${LIBDIR} -lSAInteg -o Orbit

clean:
	${RM} Orbit.dSYM

spotless: clean
	${RM} Orbit
	${RM} orbit.csv

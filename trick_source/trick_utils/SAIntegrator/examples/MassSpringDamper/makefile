
RM = rm -rf
CC = cc
CPP = c++

CXXFLAGS = -g -Wall
INCLUDE_DIRS = -I../../include
LIBDIR = ../../lib

all: MassSpringDamper

MassSpringDamper: MassSpringDamper.cpp
	$(CPP) $(CXXFLAGS) MassSpringDamper.cpp ${INCLUDE_DIRS} -L${LIBDIR} -lSAInteg -o MassSpringDamper

clean:
	${RM} MassSpringDamper.dSYM

spotless: clean
	${RM} MassSpringDamper
	${RM} msd.csv

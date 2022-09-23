
RM = rm -rf
CC = cc
CPP = c++

CXXFLAGS = -g -Wall
INCLUDE_DIRS = -I../../include
LIBDIR = ../../lib

all: DefiniteIntegral

DefiniteIntegral:
	$(CPP) $(CXXFLAGS) DefiniteIntegral.cpp ${INCLUDE_DIRS} -L${LIBDIR} -lSAInteg -o DefiniteIntegral

clean:
	${RM} DefiniteIntegral.dSYM

spotless: clean
	${RM} DefiniteIntegral

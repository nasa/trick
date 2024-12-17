
RM = rm -rf
CC = cc
CPP = c++

CXXFLAGS = -g -Wall
INCLUDE_DIRS = -I../../include
LIBDIR = ../../lib

all: DoubleIntegral

DoubleIntegral:
	$(CPP) $(CXXFLAGS) DoubleIntegral.cpp ${INCLUDE_DIRS} -L${LIBDIR} -lSAInteg -o DoubleIntegral

clean:
	${RM} DoubleIntegral.dSYM

spotless: clean
	${RM} DoubleIntegral

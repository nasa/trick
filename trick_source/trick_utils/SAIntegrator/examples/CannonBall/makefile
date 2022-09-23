
RM = rm -rf
CC = cc
CPP = c++

CXXFLAGS = -g -Wall
INCLUDE_DIRS = -I../../include
LIBDIR = ../../lib

all: CannonBall

CannonBall:
	$(CPP) $(CXXFLAGS) CannonBall.cpp ${INCLUDE_DIRS} -L${LIBDIR} -lSAInteg -o CannonBall

clean:
	${RM} CannonBall.dSYM

spotless: clean
	${RM} CannonBall
	${RM} cannon.csv

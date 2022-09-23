
RM = rm -rf
CC = cc
CPP = c++

CXXFLAGS = -g -Wall
INCLUDE_DIRS = -I../../include
LIBDIR = ../../lib

all: BouncyCannonBall

${LIBDIR}/libSAInteg.a:
	@echo "=============================================================="
	@echo "                         ATTENTION"
	@echo "The libSAInteg.a library needs to be built."
	@echo "Please \"cd\" to the SAIntegrator directory and type \"make\"."
	@echo "=============================================================="

BouncyCannonBall: ${LIBDIR}/libSAInteg.a
	$(CPP) $(CXXFLAGS) BouncyCannonBall.cpp ${INCLUDE_DIRS} -L${LIBDIR} -lSAInteg -o BouncyCannonBall

clean:
	${RM} BouncyCannonBall.dSYM

spotless: clean
	${RM} BouncyCannonBall
	${RM} cannon.csv

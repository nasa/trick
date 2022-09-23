SHELL = /bin/sh
RM = rm -rf
CC = gcc
CPP = g++

INCLUDE_DIRS = -I../include

CFLAGS = -g \
         -Wall \
         -Wshadow \
         -Wconversion \
         -Wextra \
         -Wno-unused-parameter \
         -Wno-missing-braces \
         -Wno-unused-variable

.c.o:
	${CC} ${CFLAGS} ${INCLUDE_DIRS} -c $<

.cpp.o:
	${CPP} ${CFLAGS} ${INCLUDE_DIRS} -c $<

all: IBall IRFBall rftest

IBall: IBall.o 
	${CPP} ${CFLAGS} -o $@ IBall.o -L../lib -ltrick_integ 

IRFBall: IRFBall.o 
	${CPP} ${CFLAGS} -o $@ IRFBall.o -L../lib -ltrick_integ 

rftest: rftest.o 
	${CPP} ${CFLAGS} -o $@ rftest.o -L../lib -ltrick_integ 

clean:
	${RM} *~
	${RM} *.o 
	${RM} *.dat 
	${RM} *.gpl 

spotless: clean
	${RM} IBall
	${RM} IRFBall
	${RM} rftest

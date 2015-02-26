
# $Id$

SHELL = /bin/sh
RM = rm -rf
CC = gcc
CPP = g++

INCDIRS = -I/usr/include/libxml2

CFLAGS = -g \
         -Wall \
         -Wshadow \
         -Wconversion \
         -Wextra \
         -Wno-unused-parameter \
         -Wno-missing-braces \
         -Wno-unused-variable

OBJDIR = obj
OBJS = XMLAttribute.o \
       XMLComponent.o \
       Requirement.o \
       TagRequirements.o

.cpp.o:
	${CPP} ${CFLAGS} ${INCDIRS} -c $<

all: TagRequirements

TagRequirements: ${OBJS}
	${CPP} ${CFLAGS} -o $@ ${OBJS} -lxml2

clean:
	${RM} *.o
	${RM} *~
	${RM} TagRequirements

spotless: clean

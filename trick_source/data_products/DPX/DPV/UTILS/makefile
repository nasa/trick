
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf
CC = gcc
CPP = g++

DPX_DIR  = ../..

INCDIRS = -I${DPX_DIR}

CFLAGS = -g -Wall

OBJDIR = object_${TRICK_HOST_CPU}
LIBDIR = ${DPX_DIR}/lib_${TRICK_HOST_CPU}
LIBNAME = libDPV.a
LIBOBJS = ${OBJDIR}/DPV_textbuffer.o

#.cpp.o:
#	${CPP} ${CFLAGS} ${INCDIRS} -c $<

all: ${LIBDIR}/${LIBNAME}

$(LIBOBJS): $(OBJDIR)/%.o : %.cpp | ${OBJDIR}
	$(CPP) $(CFLAGS) ${INCDIRS} -c $< -o $@

${LIBDIR}/${LIBNAME}: ${LIBOBJS} | ${LIBDIR}
	ar crs $@ ${LIBOBJS}

${OBJDIR}:
	mkdir -p ${OBJDIR}

${LIBDIR}:
	mkdir -p ${LIBDIR}

clean:
	${RM} *~
	${RM} *.o
	${RM} ${OBJDIR}

spotless: clean
	${RM} ${LIBDIR}/${LIBNAME}



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

#############################################
#			FORMATTING VARIABLES
#############################################
REMOVAL_COLOR := @ echo -e "\e[31m\c"
DEFAULT_COLOR := @ echo -e "\e[0m\c"

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
	$(REMOVAL_COLOR)
	${RM} *~
	${RM} *.o
	${RM} ${OBJDIR}
	$(DEFAULT_COLOR)
	@ echo -e "\e[32mDirectory cleaned.\e[0m"

spotless: clean
	${RM} ${LIBDIR}/${LIBNAME}


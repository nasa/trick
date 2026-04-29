RM = rm -rf
CC = cc
CPP = c++
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common


#include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib

CPP_FLAGS = -g -Wall -Wextra $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS)
INCLUDE_DIRS = -I${TRICK_HOME}/include
OBJDIR = obj
LIBDIR = lib
LIBNAME = libtrick_var_binary_parser.a
LIBOBJS = ${OBJDIR}/var_binary_parser.o

TRICK_LIB := $(TRICK_LIB_DIR)/$(LIBNAME)
# TRICK_LIB := $(TRICK_HOME)/lib/$(LIBNAME)


trick: ${TRICK_LIB}

install: ${TRICK_LIB}

all: test

test: ${LIBDIR}/${LIBNAME}
	${MAKE} -C test

$(LIBOBJS): $(OBJDIR)/%.o : src/%.cc | $(OBJDIR)
	echo $(CPP_FLAGS)
	$(CPP) $(CPP_FLAGS) ${INCLUDE_DIRS} -c $< -o $@

${LIBDIR}/${LIBNAME}: ${LIBOBJS} | ${LIBDIR}
	ar crs $@ ${LIBOBJS}

${OBJDIR}:
	mkdir -p ${OBJDIR}

${LIBDIR}: 
	mkdir -p ${LIBDIR}

${TRICK_LIB}: ${LIBDIR}/${LIBNAME}
	cp ${LIBDIR}/${LIBNAME} $(TRICK_LIB)

real_clean: clean

clean:
	${RM} -rf ${OBJDIR}
	${RM} -rf ${LIBDIR}
	${MAKE} -C test clean

spotless: 
	${RM} ${OBJDIR}
	${RM} ${LIBDIR}
	${MAKE} -C unittest spotless
	${MAKE} -C examples spotless

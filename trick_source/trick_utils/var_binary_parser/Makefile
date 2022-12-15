RM = rm -rf
CC = cc
CPP = c++

CFLAGS = -g -Wall -std=c++11
INCLUDE_DIRS = -Iinclude 
OBJDIR = obj
LIBDIR = lib
LIBNAME = var_binary_parser.a
LIBOBJS = ${OBJDIR}/var_binary_parser.o \
	  ${OBJDIR}/var_binary_parser.o

all: test examples

test: ${LIBDIR}/${LIBNAME}
	${MAKE} -C test

$(LIBOBJS): $(OBJDIR)/%.o : src/%.cc | $(OBJDIR)
	$(CPP) $(CFLAGS) ${INCLUDE_DIRS} -c $< -o $@

${LIBDIR}/${LIBNAME}: ${LIBOBJS} | ${LIBDIR}
	ar crs $@ ${LIBOBJS}

${OBJDIR}:
	mkdir -p ${OBJDIR}

${LIBDIR}:
	mkdir -p ${LIBDIR}

clean:
	${RM} *~
	${RM} ${OBJDIR}
	${MAKE} -C unittest clean
	${MAKE} -C examples clean

spotless: 
	${RM} *~
	${RM} ${OBJDIR}
	${RM} ${LIBDIR}
	${MAKE} -C unittest spotless
	${MAKE} -C examples spotless

RM = rm -rf
CC = cc
CPP = c++

CFLAGS = -g -Wall
INCLUDE_DIRS = -Iinclude
OBJDIR = obj
LIBDIR = lib
LIBNAME = libSAInteg.a
LIBOBJS = ${OBJDIR}/SAIntegrator.o

all: test examples

test: ${LIBDIR}/${LIBNAME}
	${MAKE} -C test

examples: ${LIBDIR}/${LIBNAME}
	${MAKE} -C examples

$(LIBOBJS): $(OBJDIR)/%.o : src/%.cpp | $(OBJDIR)
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
	${MAKE} -C test clean
	${MAKE} -C examples clean

spotless: 
	${RM} *~
	${RM} ${OBJDIR}
	${RM} ${LIBDIR}
	${MAKE} -C test spotless
	${MAKE} -C examples spotless

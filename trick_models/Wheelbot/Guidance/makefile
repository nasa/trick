RM = rm -rf
CPP = g++

CXXFLAGS += -DDIAGONAL_NEIGHBORS

INCLUDE_DIRS = -Iinclude
OBJDIR = obj
LIBDIR = lib
LIBNAME = libGuidance.a
LIBOBJS = $(OBJDIR)/arena.o \
	  $(OBJDIR)/navigator.o \
	  $(OBJDIR)/findpath.o

all: ${LIBDIR}/${LIBNAME}

$(LIBOBJS): $(OBJDIR)/%.o : src/%.cpp | $(OBJDIR)
	$(CPP) $(CXXFLAGS) ${INCLUDE_DIRS} -c $< -o $@

${LIBDIR}/${LIBNAME}: ${LIBOBJS} | ${LIBDIR}
	ar crs $@ ${LIBOBJS}

${OBJDIR}:
	mkdir -p ${OBJDIR}

${LIBDIR}:
	mkdir -p ${LIBDIR}

clean:
	${RM} *~
	${RM} ${OBJDIR}
	rm -f **/*.cpp~
	rm -f **/*.hh~

spotless: clean
	${RM} ${LIBDIR}/${LIBNAME}


RM = rm -rf
CPP = g++

CXXFLAGS += -g

INCLUDE_DIRS = -Iinclude -I../Guidance/include
OBJDIR = obj
LIBDIR = lib
LIBNAME = libControl.a
LIBOBJS = $(OBJDIR)/differentialDriveController.o \
	  $(OBJDIR)/vehicleController.o

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

spotless: clean
	${RM} ${LIBDIR}/${LIBNAME}


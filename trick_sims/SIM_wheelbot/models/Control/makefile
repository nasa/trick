RM = rm -rf
CPP = g++

PROJECT_DIR = ..
CXXFLAGS += -g

INCLUDE_DIRS += -I$(PROJECT_DIR)

OBJDIR = obj
LIBDIR = lib
LIBNAME = libControl.a
LIBOBJS = $(OBJDIR)/differentialDriveController.o \
          $(OBJDIR)/DCMotorSpeedController.o \
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


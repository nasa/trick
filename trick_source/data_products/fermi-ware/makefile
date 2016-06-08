include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

CC      = cc
OBJ_DIR = object_${TRICK_HOST_CPU}
DP_CFLAGS = -g -Wall -I/usr/X11R6/include -I/usr/X11/include -I${TRICK_HOME}/trick_source

ifeq ($(TRICK_HOST_TYPE), Darwin)
       DP_CFLAGS += -I/sw/include
endif
ifeq ($(TRICK_DP_FORCE_32BIT), 1)
       DP_CFLAGS += -m32
endif

OBJECTS= \
    ${OBJ_DIR}/XY.o \
    ${OBJ_DIR}/drawAxes.o \
    ${OBJ_DIR}/dragAxes.o \
    ${OBJ_DIR}/psUtils.o

LIBDIR    = object_${TRICK_HOST_CPU}
LIBNAME   = libfermi.a

all: $(LIBDIR)/$(LIBNAME)
	@ echo "Library up to date"

$(OBJ_DIR):
	@ mkdir -p $(OBJ_DIR)
	@ echo "Created $(OBJ_DIR)"

$(LIBDIR):
	- mkdir -p $(LIBDIR)


$(LIBDIR)/$(LIBNAME): $(OBJECTS) | $(LIBDIR)
	ar cr $(LIBDIR)/$(LIBNAME) $?

# For XY.c, disable "implicit declaration of function" warnings
# generated due to it's use of obsolete Motif functions that have no
# prototypes in Motif 2.x packages.  Yes, this is a naughty thing to
# do.

${OBJ_DIR}/XY.o : XY.c | $(OBJ_DIR)
	$(CC) $(DP_CFLAGS) -c ${@F:.o=.c} -o $@

${OBJ_DIR}/drawAxes.o : drawAxes.c | $(OBJ_DIR)
	$(CC) $(DP_CFLAGS) -c ${@F:.o=.c} -o $@

${OBJ_DIR}/dragAxes.o : dragAxes.c | $(OBJ_DIR)
	$(CC) $(DP_CFLAGS) -c ${@F:.o=.c} -o $@

${OBJ_DIR}/psUtils.o : psUtils.c | $(OBJ_DIR)
	$(CC) $(DP_CFLAGS) -c ${@F:.o=.c} -o $@

clean: 
	rm -rf $(LIB) $(OBJ_DIR)
	@ echo "Object files successfully removed"

real_clean: clean

$(MODEL_BIN_DIR)/XY.o: psUtils.h drawAxes.h dragAxes.h XYP.h XY.h
$(MODEL_BIN_DIR)/dragAxes.o: dragAxes.h
$(MODEL_BIN_DIR)/drawAxes.o: psUtils.h drawAxes.h

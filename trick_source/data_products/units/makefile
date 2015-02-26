
# $Id: makefile 1831 2011-07-27 21:34:24Z lin $

CC  = cc
CPP = c++

ifndef TRICK_HOST_CPU
TRICK_HOST_CPU := $(shell uname -s)
endif

ifndef TRICK_HOST_TYPE
TRICK_HOST_TYPE := $(shell uname -s)
endif

OBJ_DIR   = object_${TRICK_HOST_CPU}
LIBDIR    = ../lib_${TRICK_HOST_CPU}
LIBNAME   = libtrick_units.a
INCLUDE_DIRS = -Iinclude 
DP_CFLAGS = -g ${INCLUDE_DIRS} -fPIC

CPP_OBJECTS = $(OBJ_DIR)/UCFn.o \
              $(OBJ_DIR)/Unit.o
C_OBJECTS   = $(OBJ_DIR)/units_conv.o

ifeq ($(TRICK_DP_FORCE_32BIT), 1)
       DP_CFLAGS += -m32
endif

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

all: $(LIBDIR)/$(LIBNAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

OBJECTS: $(CPP_OBJECTS) 

ifeq ($(TRICK_FORCE_32BIT), 1)
$(LIBDIR)/$(LIBNAME): $(CPP_OBJECTS) $(C_OBJECTS) $(LIB_DIR)
	ar crs $(LIBDIR)/$(LIBNAME) $?
else
ifeq ($(TRICK_HOME),)
$(LIBDIR)/$(LIBNAME): $(CPP_OBJECTS) $(C_OBJECTS) $(LIB_DIR)
	ar crs $(LIBDIR)/$(LIBNAME) $?
else
$(LIBDIR)/$(LIBNAME): $(LIBDIR)
	ln -sf ${TRICK_HOME}/lib_${TRICK_HOST_CPU}/libtrick_units.a $(LIBDIR)/$(LIBNAME)
endif
endif

depend:
	@ echo "No depend rule for this directory"

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(LIBDIR)/$(LIBNAME)

real_clean: clean

$(CPP_OBJECTS) : $(OBJ_DIR)/%.o : src/%.cpp | $(OBJ_DIR)
	$(CC) $(DP_CFLAGS) -c $< -o $@

$(C_OBJECTS) : $(OBJ_DIR)/%.o : src/%.c | $(OBJ_DIR)
	$(CC) $(DP_CFLAGS) -c $< -o $@


#----------------------------
# Dependencies

#######################################################################
# To update dependencies:
# - Save changes (if any)
# - run "make depend"
#######################################################################


include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

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
INCLUDE_DIRS = -I${TRICK_HOME}/include
DP_CFLAGS = -g ${INCLUDE_DIRS} -fPIC $(UDUNITS_INCLUDES)

CPP_OBJECTS = \
 $(OBJ_DIR)/init_units_system.o \
 $(OBJ_DIR)/map_trick_units_to_udunits.o

C_OBJECTS = $(OBJ_DIR)/units_conv.o

ifeq ($(TRICK_DP_FORCE_32BIT), 1)
       DP_CFLAGS += -m32
endif

#############################################
#			FORMATTING VARIABLES
#############################################
REMOVAL_COLOR := @ echo -e "\e[31m\c"
DEFAULT_COLOR := @ echo -e "\e[0m\c"

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

all: $(LIBDIR)/$(LIBNAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIBDIR):
	mkdir -p $(LIBDIR)

OBJECTS: $(CPP_OBJECTS) 

$(LIBDIR)/$(LIBNAME): $(CPP_OBJECTS) $(C_OBJECTS) $(LIBDIR)
	ar crs $(LIBDIR)/$(LIBNAME) $(CPP_OBJECTS) $(C_OBJECTS)

clean:
	$(REMOVAL_COLOR)
	rm -rf $(OBJ_DIR)
	rm -f $(LIBDIR)/$(LIBNAME) 
	$(DEFAULT_COLOR)
	@ echo -e "\e[32mDirectory cleaned.\e[0m"

real_clean: clean

$(CPP_OBJECTS) : $(OBJ_DIR)/%.o : %.cpp | $(OBJ_DIR)
	$(CC) $(DP_CFLAGS) -c $< -o $@

$(C_OBJECTS) : $(OBJ_DIR)/%.o : %.c | $(OBJ_DIR)
	$(CC) $(DP_CFLAGS) -c $< -o $@


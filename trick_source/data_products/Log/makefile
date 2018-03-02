
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

CC        = c++

ifndef TRICK_HOST_CPU
TRICK_HOST_CPU := $(shell uname -s)
endif

ifndef TRICK_HOST_TYPE
TRICK_HOST_TYPE := $(shell uname -s)
endif

OBJ_DIR   = object_${TRICK_HOST_CPU}
LIBDIR    = ../lib_${TRICK_HOST_CPU}
LIBNAME   = liblog.a
INCLUDE_DIRS = -I.. -I${TRICK_HOME}/include
DP_CFLAGS = -g ${INCLUDE_DIRS} -fPIC $(UDUNITS_INCLUDES)
CPP_OBJECTS  = $(OBJ_DIR)/log.o \
            $(OBJ_DIR)/multiLog.o \
            $(OBJ_DIR)/trick_byteswap.o \
            $(OBJ_DIR)/parseLogHeader.o \
            $(OBJ_DIR)/Csv.o \
            $(OBJ_DIR)/TrickBinary.o \
            $(OBJ_DIR)/MatLab.o \
            $(OBJ_DIR)/MatLab4.o \
            $(OBJ_DIR)/DataStream.o \
            $(OBJ_DIR)/DataStreamFactory.o \
            $(OBJ_DIR)/DataStreamGroup.o \
            $(OBJ_DIR)/Delta.o \
            $(OBJ_DIR)/ExternalProgram.o

ifneq ($(HDF5),)
 $(info ---Including HDF5---)
 CPP_OBJECTS += $(OBJ_DIR)/TrickHDF5.o
 $(OBJ_DIR)/TrickHDF5.o: DP_CFLAGS += -I$(HDF5)/include -DHDF5
 $(OBJ_DIR)/DataStreamFactory.o: DP_CFLAGS += -I$(HDF5)/include -DHDF5
endif

ifeq ($(TRICK_HOST_TYPE), Linux)
 MAKEDEP_CPP_INC = -Iinclude ${INCLUDE_DIRS}
 DP_CFLAGS += -Wall
endif

ifeq ($(TRICK_HOST_TYPE), Darwin)
endif

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
	- mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBNAME): $(CPP_OBJECTS) | $(LIBDIR)
	ar crs $(LIBDIR)/$(LIBNAME) $?

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(LIBDIR)/$(LIBNAME)

real_clean: clean

$(CPP_OBJECTS) : $(OBJ_DIR)/%.o : %.cpp | $(OBJ_DIR)
	$(CC) $(DP_CFLAGS) -c $< -o $@

#----------------------------
# Dependencies

#######################################################################
# To update dependencies:
# - Save changes (if any)
# - run "make depend"
#######################################################################
# DO NOT DELETE

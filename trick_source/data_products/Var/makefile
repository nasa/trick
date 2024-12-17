
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
LIBNAME   = libvar.a
INCLUDE_DIRS = -I${TRICK_HOME}/include
DP_CFLAGS = -g ${INCLUDE_DIRS} -fPIC

OBJECTS0  = $(OBJ_DIR)/var.o

ifeq ($(TRICK_HOST_TYPE), Linux)
 MAKEDEP_CPP_INC = -Iinclude ${INCLUDE_DIRS}
 DP_CFLAGS += -Wall
endif

ifeq ($(TRICK_HOST_TYPE), Darwin)
 DP_CFLAGS += -Wall
endif

ifeq ($(TRICK_DP_FORCE_32BIT), 1)
       DP_CFLAGS += -m32
endif

#
# Make information
#
MAKEFILE    = makefile
AWKTEMPFILE = awk.temp
MAKEDEPEND  = makedepend

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

all: $(LIBDIR)/$(LIBNAME)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(LIBDIR):
	- mkdir -p $(LIBDIR)

$(LIBDIR)/$(LIBNAME): $(OBJECTS0) | $(LIBDIR)
	ar cr $(LIBDIR)/$(LIBNAME) $?

depend:
	@ $(MAKEDEPEND) -s"# DO NOT DELETE" -f $(MAKEFILE) 1>/dev/null 
	@ touch $(AWKTEMPFILE)
	@ $(CC) -MM $(DP_CFLAGS) $(C_SRC) $(CPP_SRC) $(IO_SRC) >> $(AWKTEMPFILE)
	@ echo "Appended dependencies to $(MAKEFILE)"

clean:
	rm -rf $(OBJ_DIR)
	rm -f $(LIBDIR)/$(LIBNAME) 

real_clean: clean 

$(OBJ_DIR)/var.o: var.cpp | $(OBJ_DIR)
	$(CC) ${DP_CFLAGS} -c var.cpp  -o $(OBJ_DIR)/var.o


#----------------------------
# Dependencies

#######################################################################
# To update dependencies:
# - Save changes (if any)
# - run "make depend"
#######################################################################

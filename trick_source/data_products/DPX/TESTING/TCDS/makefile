
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

ifneq ($(HDF5),)
ifneq ($(HDF5),/usr)
 HDF5_LIB = -L$(HDF5)/lib -L$(HDF5)/lib64 -lhdf5_hl -lhdf5 -lsz
else
 HDF5_LIB = -lhdf5_hl -lhdf5
endif
endif

SHELL = /bin/sh
RM = rm -rf
CC = gcc
CPP = g++
PWD = $(shell pwd)
DEV_NULL = > /dev/null 2>&1
GCC = $(shell which gcc)

DPX_DIR = ${TRICK_HOME}/trick_source/data_products/DPX
# Save number plus first decimal place.
GCC_VERSION = $(shell ${GCC} -dumpversion 2>&1 | sed -e 's/\([0-9]\.[0-9]*\).*/\1/')

INCDIRS = -I${TRICK_HOME}/trick_source \
          -I${TRICK_HOME}/trick_source/data_products \
          -I${DPX_DIR} \
          -I${DPX_DIR}/TESTING

CFLAGS = -g -Wall ${INCDIRS}

OBJS = tcdstest.o
DS_exercisor_OBJS = ../DS_exercisor/DS_exercisor.o

ifeq ($(TRICK_HOST_TYPE), Linux)
    LIBRTDEF = -lrt
else
    LIBRTDEF =
endif

ifeq ($(TRICK_HOST_TYPE), Darwin)
     MAKEDEP_CPP_INC = -Iinclude ${INCDIRS} -I/usr/include/gcc/darwin/${GCC_VERSION}/g++-v3 \
      -I/usr/include/gcc/darwin/${GCC_VERSION}/g++-v3/ppc-darwin \
      -I/usr/include/gcc/darwin/default/g++-v3/backward \
      -I/usr/include/gcc/darwin/${GCC_VERSION}
else
     MAKEDEP_CPP_INC = -Iinclude ${INCDIRS} -I/usr/include/g++-3
endif

TRICK_UNIT_LIBS = -L${TRICK_HOME}/lib_${TRICK_HOST_CPU} -ltrick_units
DP_LIBS         = -L${TRICK_HOME}/lib_${TRICK_HOST_CPU} \
                  -L${TRICK_HOME}/trick_source/data_products/lib_${TRICK_HOST_CPU} \
                  -llog -lvar -leqparse
MODEL_LIBS      = -L/usr/lib64 -L/usr/lib -lxml2 \
                  -L${DPX_DIR}/lib_${TRICK_HOST_CPU} -lDPM
CONTROLLER_LIBS = -L${DPX_DIR}/lib_${TRICK_HOST_CPU} -lDPC \
                  ${MODEL_LIBS} ${DP_LIBS} ${TRICK_UNIT_LIBS} ${LIBRTDEF} -ldl \
                  -L/usr/lib64 -L/usr/lib -lz ${HDF5_LIB}

#
# Make information
#
MAKEFILE    = makefile
AWKTEMPFILE = awk.temp
MAKEDEPEND  = makedepend
FTNDEPEND   = ftn_depend

#############################################################################
##                        FILE NAME DEFINITIONS                            ##
#############################################################################

C_SRC = $(wildcard $(SRC_DIR)*.c)
C_OBJS = $($(addprefix $(OBJ_DIR)/,$(notdir $(subst .c,.o,$(C_SRC)))))

CPP_C_SRC = $(wildcard $(SRC_DIR)*.C)
CPP_CC_SRC = $(wildcard $(SRC_DIR)*.cc)
CPP_CPP_SRC = $(wildcard $(SRC_DIR)*.cpp)
CPP_CXX_SRC = $(wildcard $(SRC_DIR)*.cxx)
CPP_CPLUS_SRC = $(wildcard $(SRC_DIR)*.c++)
CPP_SRC = $(CPP_C_SRC) $(CPP_CC_SRC) $(CPP_CPP_SRC) $(CPP_CXX_SRC) $(CPP_CPLUS_SRC)
CPP_C_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .C,.o,$(CPP_C_SRC))))
CPP_CC_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .cc,.o,$(CPP_CC_SRC))))
CPP_CPP_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .cpp,.o,$(CPP_CPP_SRC))))
CPP_CXX_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .cxx,.o,$(CPP_CXX_SRC))))
CPP_CPLUS_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .c++,.o,$(CPP_CPLUS_SRC))))
CPP_OBJS = $(CPP_C_OBJS) $(CPP_CC_OBJS) $(CPP_CPP_OBJS) $(CPP_CXX_OBJS) $(CPP_CPLUS_OBJS)

F_SRC = $(wildcard $(SRC_DIR)*.f)
F_OBJS = $(addprefix $(OBJ_DIR)/,$(notdir $(subst .f,.o,$(F_SRC))))

IO_C_SRC = $(wildcard $(IO_SRC_DIR)*.c)
IO_C_OBJS =$(subst io_src,$(OBJ_DIR),$(subst .c,.o,$(IO_C_SRC)))

IO_CPP_SRC = $(wildcard $(IO_SRC_DIR)*.cpp)
IO_CPP_OBJS =$(subst io_src,$(OBJ_DIR),$(subst .cpp,.o,$(IO_CPP_SRC)))

IO_SRC = $(IO_C_SRC) $(IO_CPP_SRC)
IO_OBJS = $(IO_C_OBJS) $(IO_CPP_OBJS)

OBJECT_FILES = $(IO_OBJS) $(C_OBJS) $(CPP_OBJS) $(F_OBJS)

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

.c.o:
	${CC} ${CFLAGS} -c $<

.cpp.o:
	${CPP} ${CFLAGS} -c $<

all: tcdstest

tcdstest: ${OBJS} ${DS_exercisor_OBJS}
	${CPP} ${CFLAGS} -o $@ ${OBJS} ${CONTROLLER_LIBS} ../DS_exercisor/DS_exercisor.o

${DS_exercisor_OBJS}:
	${MAKE} -C ../DS_exercisor

depend:
	@ $(MAKEDEPEND) -s"# DO NOT DELETE" -f $(MAKEFILE) 1>/dev/null
	@ touch $(AWKTEMPFILE)
	@ $(CC) -MM $(TRICK_CFLAGS) $(MAKEDEP_CPP_INC) $(C_SRC) $(CPP_SRC) $(IO_SRC) >> $(AWKTEMPFILE)
	@ depend_cp $(AWKTEMPFILE) >> $(MAKEFILE)
	@ $(RM) -f $(AWKTEMPFILE) $(AWKTEMPFILE).bak
	@ echo "Appended dependencies to $(MAKEFILE)"

clean:
	${RM} *~
	${RM} *.o

spotless: clean
	${RM} tcdstest


# Dependencies

#######################################################################
# To update dependencies:
# - Save changes (if any)
# - run "make depend"
#######################################################################
# DO NOT DELETE
object_${TRICK_HOST_CPU}/tcdstest.o: tcdstest.cpp \
 ${TRICK_HOME}/trick_source/trick_utils/units/include/Unit.hh \
 ${TRICK_HOME}/trick_source/trick_utils/units/include/units_conv.h \
 ${TRICK_HOME}/trick_source/trick_utils/units/include/UCFn.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStream.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStreamFactory.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStream.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/Csv.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/FixedAscii.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/OctaveAscii.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/OctaveBinary.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/TrickBinary.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/TrickBinary04.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/log.h \
 ${TRICK_HOME}/trick_source/data_products/Var/var.h \
 ${TRICK_HOME}/trick_source/data_products/units/include/Unit.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/trick_byteswap.h \
 ${TRICK_HOME}/trick_source/data_products/Log/MatLab.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/MatLab4.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/ExternalProgram.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStreamGroup.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/Delta.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPC/DPC_TimeCstrDataStream.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_time_constraints.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStream.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_time_constraints.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/TESTING/DS_exercisor/DS_exercisor.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStream.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStreamFactory.hh
object_${TRICK_HOST_CPU}/tcdstest.o: tcdstest.cpp \
 ${TRICK_HOME}/trick_source/trick_utils/units/include/Unit.hh \
 ${TRICK_HOME}/trick_source/trick_utils/units/include/units_conv.h \
 ${TRICK_HOME}/trick_source/trick_utils/units/include/UCFn.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStream.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStreamFactory.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStream.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/Csv.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/FixedAscii.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/OctaveAscii.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/OctaveBinary.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/TrickBinary.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/TrickBinary04.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/log.h \
 ${TRICK_HOME}/trick_source/data_products/Var/var.h \
 ${TRICK_HOME}/trick_source/data_products/units/include/Unit.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/trick_byteswap.h \
 ${TRICK_HOME}/trick_source/data_products/Log/MatLab.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/MatLab4.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/ExternalProgram.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStreamGroup.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/Delta.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPC/DPC_TimeCstrDataStream.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_time_constraints.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStream.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_time_constraints.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/TESTING/DS_exercisor/DS_exercisor.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStream.hh \
 ${TRICK_HOME}/trick_source/data_products/Log/DataStreamFactory.hh

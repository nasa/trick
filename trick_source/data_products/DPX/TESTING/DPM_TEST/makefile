
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

SHELL = /bin/sh
RM = rm -rf
CC = gcc
CPP = g++
PWD = $(shell pwd)
DEV_NULL = > /dev/null 2>&1
GCC = $(shell which gcc)

DPX_DIR = ${TRICK_HOME}/trick_source/data_products/DPX

INCDIRS = -I/usr/include/libxml2 \
          -I/usr/X11R6/include \
          -I${TRICK_HOME}/trick_source \
          -I${TRICK_HOME}/trick_source/data_products/Apps/FPlot \
          -I${TRICK_HOME}/trick_source/data_products/DPX \
	   ${MOTIF_INCDIR}

CFLAGS = -g -Wall ${INCDIRS}

LIBDIR = ${DPX_DIR}/lib_${TRICK_HOST_CPU}
# Save number plus first decimal place.
GCC_VERSION = $(shell ${GCC} -dumpversion 2>&1 | sed -e 's/\([0-9]\.[0-9]*\).*/\1/')

ifeq ($(TRICK_HOST_TYPE), Darwin)
     MAKEDEP_CPP_INC = -Iinclude ${INCDIRS} -I/usr/include/gcc/darwin/${GCC_VERSION}/g++-v3 \
      -I/usr/include/gcc/darwin/${GCC_VERSION}/g++-v3/ppc-darwin \
      -I/usr/include/gcc/darwin/default/g++-v3/backward \
      -I/usr/include/gcc/darwin/${GCC_VERSION}
     XLIBS = -L/sw/lib -lXm -L/usr/X11R6/lib -lXt -lX11
else
     MAKEDEP_CPP_INC = -Iinclude ${INCDIRS} -I/usr/include/g++-3
     XLIBS = -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lXm -lXt -lX11
endif
TRICK_UNIT_LIBS = -L${TRICK_HOME}/lib_${TRICK_HOST_CPU} -ltrick_units
MODEL_LIBS      = -lxml2 \
                  -L${DPX_DIR}/lib_${TRICK_HOST_CPU} -lDPM \
                  ${TRICK_UNIT_LIBS}

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

all: model_test

always_make:

model_test: model_test.o ${LIBDIR}/libDPM.a
	@echo "===== Making model_test ====="
	${CPP} -o $@ model_test.o ${MODEL_LIBS}

${LIBDIR}/libDPM.a:
	@echo "===== Making libDPM.a ====="
	$(MAKE) -C ${DPX_DIR}/DPM

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
	${RM} model_test


# Dependencies

#######################################################################
# To update dependencies:
# - Save changes (if any)
# - run "make depend"
#######################################################################
# DO NOT DELETE
object_${TRICK_HOST_CPU}/model_test.o: model_test.cpp \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_product.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_component.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_attribute.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_extfn.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_inputs.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_var.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_outputs.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_measurement.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_page.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_relation.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_axis.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_curve.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_time_constraints.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_table.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_column.hh
object_${TRICK_HOST_CPU}/model_test.o: model_test.cpp \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_product.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_component.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_attribute.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_extfn.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_inputs.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_var.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_outputs.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_measurement.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_page.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_relation.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_axis.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_curve.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_time_constraints.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_table.hh \
 ${TRICK_HOME}/trick_source/data_products/DPX/DPM/DPM_column.hh

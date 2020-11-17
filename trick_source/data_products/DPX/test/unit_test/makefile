
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf
CC = gcc
CPP = g++

DPX_DIR = ${TRICK_HOME}/trick_source/data_products/DPX
DS_DIR  = ${TRICK_HOME}/trick_source/data_products

INCDIRS = -I$(GTEST_HOME)/include \
          -I/usr/include/libxml2 \
          -I/usr/X11R6/include \
          -I${TRICK_HOME}/include \
          -I${TRICK_HOME}/trick_source/data_products \
          -I${TRICK_HOME}/trick_source/data_products/Apps/FPlot \
          -I${TRICK_HOME}/trick_source/data_products/DPX \
          ${MOTIF_INCDIR}

CFLAGS = -g -Wall -Wextra ${INCDIRS} $(UDUNITS_INCLUDES)

LIB_DPX_DIR = ${DPX_DIR}/lib_${TRICK_HOST_CPU}
LIB_DS_DIR  = ${DS_DIR}/lib_${TRICK_HOST_CPU}
# Save number plus first decimal place.

ifeq ($(TRICK_HOST_TYPE), Linux)
    LIBRTDEF = -lrt
else
    LIBRTDEF =
endif

ifeq ($(TRICK_HOST_TYPE), Darwin)
    CFLAGS += -DGTEST_HAS_TR1_TUPLE=0
    MOTIF_INCDIR = -I/sw/include
    XLIBS = -L/sw/lib -lXm -L/usr/X11R6/lib -L/usr/X11/lib
    SDK_DIR = $(shell xcrun --show-sdk-path)
    INCDIRS += -I${SDK_DIR}/usr/include/libxml2
else
    MOTIF_INCDIR = -I/usr/X11R6/lib64 -I/usr/X11R6/lib
    XLIBS = -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lXm -lXt -lX11 -ldl
endif

GTEST_LIBS = -L${GTEST_HOME}/lib64 -L${GTEST_HOME}/lib -lgtest -lgtest_main

DP_LIBS         = -L${TRICK_HOME}/trick_source/data_products/lib_${TRICK_HOST_CPU} \
                  -llog -lvar -leqparse -ltrick_units
MODEL_LIBS      = -lxml2 \
                  -L${DPX_DIR}/lib_${TRICK_HOST_CPU} -lDPM \
                  -L${TRICK_HOME}/trick_source/data_products/lib_${TRICK_HOST_CPU} -ltrick_units \
                  ${GTEST_LIBS} -lpthread -ludunits2
CONTROLLER_LIBS = -lDPC \
                  ${MODEL_LIBS} ${DP_LIBS} \
                  ${XLIBS} ${LIBRTDEF} \
                  -L/usr/lib64 -L/usr/lib -lz ${HDF5_LIB}
DS_LIBS         = ${DP_LIBS} -ldl ${CONTROLLER_LIBS}


TESTS = DPM_test \
		DPC_test \
		DS_test

#############################################################################
##                            MODEL TARGETS                                ##
#############################################################################

.c.o:
	${CC} ${CFLAGS} -c $<

.cpp.o:
	${CPP} ${CFLAGS} -c $<

all: $(TESTS)

test : $(TESTS)
	./DPC_test --gtest_output=xml:${TRICK_HOME}/trick_test/DataProducts_C.xml
	./DPM_test --gtest_output=xml:${TRICK_HOME}/trick_test/DataProducts_M.xml
	./DS_test  --gtest_output=xml:${TRICK_HOME}/trick_test/DataStream.xml

DPM_test: DPM_test.o ${LIB_DPX_DIR}/libDPM.a
	@echo "===== Making DPM_test ====="
	${CPP} -o $@ DPM_test.o ${MODEL_LIBS}

DPC_test: DPC_test.o test_view.o ${LIB_DPX_DIR}/libDPM.a ${LIB_DPX_DIR}/libDPC.a
	@echo "===== Making DPC_test ====="
	${CPP} -o $@ DPC_test.o test_view.o ${CONTROLLER_LIBS}

DS_test:  DS_test.o ${LIB_DS_DIR}/liblog.a
	@echo "===== Making DS_test ======" 
	${CPP} -o $@  DS_test.o ${DS_LIBS}

${LIB_DPX_DIR}/libDPM.a:
	@echo "===== Making libDPM.a ====="
	$(MAKE) -C ${DPX_DIR}/DPM

${LIB_DPX_DIR}/libDPC.a:
	@echo "===== Making libDPC.a ====="
	$(MAKE) -C ${DPX_DIR}/DPC

clean:
	${RM} *~
	${RM} $(TESTS) *.o


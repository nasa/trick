
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RM = rm -rf
CC = gcc
CPP = g++

# TRICK_SRC_HOME is separate from TRICK_HOME so this test can run with
# TRICK_HOME pointed at a staged (out-of-source) CMake install while still
# finding data_products' headers, which only exist in the source tree — a
# CMake build compiles in place from source without copying headers into
# the build tree. Defaults to TRICK_HOME, preserving in-source `make`
# behavior exactly (TRICK_HOME already is the source root there).
TRICK_SRC_HOME ?= ${TRICK_HOME}

DPX_DIR = ${TRICK_SRC_HOME}/trick_source/data_products/DPX
DS_DIR  = ${TRICK_SRC_HOME}/trick_source/data_products

INCDIRS = -I$(GTEST_HOME)/include \
          -I/usr/include/libxml2 \
          -I/usr/X11R6/include \
          -I${TRICK_HOME}/include \
          -I${DS_DIR} \
          -I${DS_DIR}/Apps/FPlot \
          -I${DPX_DIR} \
          ${MOTIF_INCDIR}

CFLAGS = -g -Wall -Wextra ${INCDIRS} $(UDUNITS_INCLUDES)

LIB_DPX_DIR = ${DPX_DIR}/lib_${TRICK_HOST_CPU}
LIB_DS_DIR  = ${DS_DIR}/lib_${TRICK_HOST_CPU}
# Save number plus first decimal place.

# Per-library search dirs, each independently overridable and defaulting to
# the shared lib_$(TRICK_HOST_CPU) dirs above (identical to historical
# behavior under in-source `make`). A CMake-driven test run overrides these
# individually, since CMake builds each library into its own target output
# directory (DPM/libDPM.a, DPC/libDPC.a, Log/liblog.a, ...) rather than one
# shared directory.
LIB_LOG_DIR     = ${LIB_DS_DIR}
LIB_VAR_DIR     = ${LIB_DS_DIR}
LIB_EQPARSE_DIR = ${LIB_DS_DIR}
LIB_UNITS_DIR   = ${LIB_DS_DIR}
LIB_DPM_DIR     = ${LIB_DPX_DIR}
LIB_DPC_DIR     = ${LIB_DPX_DIR}

ifeq ($(TRICK_HOST_TYPE), Linux)
    LIBRTDEF = -lrt
else
    LIBRTDEF =
endif

ifeq ($(TRICK_HOST_TYPE), Darwin)
    CFLAGS += -std=c++17
    MOTIF_INCDIR = -I/sw/include
    XLIBS = -L/sw/lib -lXm -L/usr/X11R6/lib -L/usr/X11/lib
    SDK_DIR = $(shell xcrun --show-sdk-path)
    INCDIRS += -I${SDK_DIR}/usr/include/libxml2
else
    MOTIF_INCDIR = -I/usr/X11R6/lib64 -I/usr/X11R6/lib
    XLIBS = -L/usr/X11R6/lib64 -L/usr/X11R6/lib -lXm -lXt -lX11 -ldl
endif

GTEST_LIBS = -L${GTEST_HOME}/lib64 -L${GTEST_HOME}/lib -lgtest -lgtest_main

DP_LIBS         = -L${LIB_LOG_DIR} -llog \
                  -L${LIB_VAR_DIR} -lvar \
                  -L${LIB_EQPARSE_DIR} -leqparse \
                  -L${LIB_UNITS_DIR} -ltrick_units
MODEL_LIBS      = -lxml2 \
                  -L${LIB_DPM_DIR} -lDPM \
                  -L${LIB_UNITS_DIR} -ltrick_units \
                  ${GTEST_LIBS} -lpthread -ludunits2 ${UDUNITS_LDFLAGS}
CONTROLLER_LIBS = -L${LIB_DPC_DIR} -lDPC \
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

DPM_test: DPM_test.o ${LIB_DPM_DIR}/libDPM.a
	@echo "===== Making DPM_test ====="
	${CPP} -o $@ DPM_test.o ${MODEL_LIBS}

DPC_test: DPC_test.o test_view.o ${LIB_DPM_DIR}/libDPM.a ${LIB_DPC_DIR}/libDPC.a
	@echo "===== Making DPC_test ====="
	${CPP} -o $@ DPC_test.o test_view.o ${CONTROLLER_LIBS}

DS_test:  DS_test.o ${LIB_LOG_DIR}/liblog.a
	@echo "===== Making DS_test ======"
	${CPP} -o $@  DS_test.o ${DS_LIBS}

${LIB_DPM_DIR}/libDPM.a:
	@echo "===== Making libDPM.a ====="
	$(MAKE) -C ${DPX_DIR}/DPM

${LIB_DPC_DIR}/libDPC.a:
	@echo "===== Making libDPC.a ====="
	$(MAKE) -C ${DPX_DIR}/DPC

clean:
	${RM} *~
	${RM} $(TESTS) *.o

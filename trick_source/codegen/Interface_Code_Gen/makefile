
# The config_${HOST_TYPE}.mk file provides LLVM_HOME
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

CC := $(shell $(LLVM_HOME)/bin/llvm-config --bindir)/clang
CXX := $(shell $(LLVM_HOME)/bin/llvm-config --bindir)/clang++
CXXFLAGS := -g -I$(shell $(LLVM_HOME)/bin/llvm-config --includedir) -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-rtti

CLANG_MINOR_GTEQ5 := $(shell expr `$(LLVM_HOME)/bin/llvm-config --version | cut -f2 -d. ` \>= 5 )

LLVMLDFLAGS := $(shell $(LLVM_HOME)/bin/llvm-config --ldflags)

OBJ_DIR :=  object_$(TRICK_HOST_CPU)

ifeq ($(IS_CC_CLANG), 0)
CXXFLAGS += -DGCC_MAJOR=$(GCC_MAJOR) -DGCC_MINOR=$(GCC_MINOR)
endif

ICG := ${TRICK_HOME}/bin/trick-ICG

SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix $(OBJ_DIR)/, $(subst .cpp,.o,$(SOURCES)))

# Include the units conversion source from trick_utils.
UNITS_CONV_SRC = ${TRICK_HOME}/trick_source/trick_utils/units/src/units_conv.c
UNITS_CONV_INCLUDE = -I${TRICK_HOME}/include
UNITS_CONV_OBJ = $(OBJ_DIR)/units_conv.o

CLANGLIBS = \
    -lclangFrontend \
    -lclangDriver \
    -lclangSerialization \
    -lclangParse \
    -lclangSema \
    -lclangAnalysis \
    -lclangEdit \
    -lclangAST \
    -lclangLex \
    -lclangBasic \

ifeq ($(TRICK_HOST_TYPE),Linux)
CLANGLIBS += $(shell $(LLVM_HOME)/bin/llvm-config --libs)
ifeq ($(CLANG_MINOR_GTEQ5),1)
CXXFLAGS += -std=c++11
# Fedora 21 adds -ledit as a system lib, but it isn't installed, or required.
CLANGLIBS += $(filter-out -ledit,$(shell $(LLVM_HOME)/bin/llvm-config --system-libs))
endif
endif

ifeq ($(TRICK_HOST_TYPE),Darwin)
CXXFLAGS += -std=c++11
CLANGLIBS += -lLLVMOption -lLLVMMCParser -lLLVMBitReader -lLLVMMC -lLLVMSupport $(shell $(LLVM_HOME)/bin/llvm-config --system-libs)
CLANGLIBS += -lc++abi
endif

all: $(ICG)

$(ICG): $(OBJECTS) $(UNITS_CONV_OBJ)
	$(CXX) -o $@ $(OBJECTS) $(UNITS_CONV_OBJ) $(LLVMLDFLAGS) $(CLANGLIBS)

# Only FieldDescription.cpp includes the units conversion header.
$(OBJ_DIR)/FieldDescription.o : CXXFLAGS += $(UNITS_CONV_INCLUDE)
$(OBJ_DIR)/HeaderSearchDirs.o : CXXFLAGS += -DLLVM_HOME=\"${LLVM_HOME}\"
$(OBJ_DIR)/main.o : CXXFLAGS += $(UNITS_CONV_INCLUDE)

$(OBJECTS): $(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(UNITS_CONV_OBJ): $(UNITS_CONV_SRC)
	$(CC) $(UNITS_CONV_INCLUDE) -c $< -o $@

$(OBJECTS) $(UNITS_CONV_OBJ) : | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(ICG)

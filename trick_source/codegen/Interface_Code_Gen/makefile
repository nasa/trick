
# The config_${HOST_TYPE}.mk file provides LLVM_HOME
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

CC := $(CLANG)
CXX := $(CLANG)++
CXXFLAGS := -g -I$(shell $(LLVM_HOME)/bin/llvm-config --includedir) -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-rtti

CLANG_MINOR_GTEQ5 := $(shell expr `$(LLVM_HOME)/bin/llvm-config --version | cut -f2 -d. ` \>= 5 )

LLVMLDFLAGS := $(shell $(LLVM_HOME)/bin/llvm-config --ldflags) -ludunits2

OBJ_DIR :=  object_$(TRICK_HOST_CPU)

ifeq ($(IS_CC_CLANG), 0)
CXXFLAGS += -DGCC_MAJOR=$(GCC_MAJOR) -DGCC_MINOR=$(GCC_MINOR)
endif

ICG := ${TRICK_HOME}/bin/trick-ICG

SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix $(OBJ_DIR)/, $(subst .cpp,.o,$(SOURCES)))

UDUNITS_OBJS = \
    $(OBJ_DIR)/map_trick_units_to_udunits.o

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
CXXFLAGS += -I/usr/include/udunits2
endif

ifeq ($(TRICK_HOST_TYPE),Darwin)
CXXFLAGS += -std=c++11
CLANGLIBS += -lLLVMOption -lLLVMMCParser -lLLVMBitReader -lLLVMMC -lLLVMSupport $(shell $(LLVM_HOME)/bin/llvm-config --system-libs)
CLANGLIBS += -lc++abi
endif

all: $(ICG)

$(ICG): $(OBJECTS) $(UDUNITS_OBJS)
	$(CXX) -o $@ $(OBJECTS) $(UNITS_CONV_OBJ) $(UDUNITS_OBJS) $(LLVMLDFLAGS) $(CLANGLIBS)

# Only FieldDescription.cpp includes the units conversion header.
$(OBJ_DIR)/FieldDescription.o : CXXFLAGS += $(UNITS_CONV_INCLUDE)
$(OBJ_DIR)/HeaderSearchDirs.o : CXXFLAGS += -DLLVM_HOME=\"${LLVM_HOME}\"
$(OBJ_DIR)/main.o : CXXFLAGS += $(UNITS_CONV_INCLUDE)

$(OBJECTS): $(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(UDUNITS_OBJS): $(OBJ_DIR)/%.o : ${TRICK_HOME}/trick_source/sim_services/UdUnits/%.cpp
	$(CXX) -c $< -o $@

$(OBJECTS) $(UNITS_CONV_OBJ) : | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(ICG)

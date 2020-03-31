TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../../..)
# The config_${HOST_TYPE}.mk file provides LLVM_HOME
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

CXXFLAGS := -g -I$(shell $(LLVM_HOME)/bin/llvm-config --includedir) -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -fno-rtti $(UDUNITS_INCLUDES)

CLANG_MAJOR := $(shell $(LLVM_HOME)/bin/llvm-config --version | cut -f1 -d.)
CLANG_MINOR := $(shell $(LLVM_HOME)/bin/llvm-config --version | cut -f2 -d.)
CLANG_PATCHLEVEL := $(shell $(LLVM_HOME)/bin/llvm-config --version | cut -f3 -d.)
# check to see if version is greater than 3.5
CLANG_MINOR_GTEQ5 := $(shell [ $(CLANG_MAJOR) -gt 3 -o \( $(CLANG_MAJOR) -eq 3 -a $(CLANG_MINOR) -ge 5 \) ] && echo 1)

CLANG_MAJOR_GTEQ10 := $(shell [ $(CLANG_MAJOR) -ge 10 ] && echo 1)
ifeq ($(CLANG_MAJOR_GTEQ10),1)
CXXFLAGS += -std=c++14
else
CXXFLAGS += -std=c++11
endif

LLVMLDFLAGS := $(shell $(LLVM_HOME)/bin/llvm-config --ldflags) $(UDUNITS_LDFLAGS)

OBJ_DIR :=  object_$(TRICK_HOST_CPU)

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

CXXFLAGS += -DLIBCLANG_MAJOR=$(CLANG_MAJOR) -DLIBCLANG_MINOR=$(CLANG_MINOR)
ifneq ($(CLANG_PATCHLEVEL),)
CXXFLAGS += -DLIBCLANG_PATCHLEVEL=$(CLANG_PATCHLEVEL)
endif

ifeq ($(TRICK_HOST_TYPE),Linux)
CLANGLIBS += $(shell $(LLVM_HOME)/bin/llvm-config --libs)
ifeq ($(CLANG_MINOR_GTEQ5),1)
# Fedora 21 adds -ledit as a system lib, but it isn't installed, or required.
CLANGLIBS += $(filter-out -ledit,$(shell $(LLVM_HOME)/bin/llvm-config --system-libs))
endif
endif

ifeq ($(TRICK_HOST_TYPE),Darwin)
CLANGLIBS += $(shell $(LLVM_HOME)/bin/llvm-config --libs)
CLANGLIBS += $(shell $(LLVM_HOME)/bin/llvm-config --system-libs)
CLANGLIBS += -lc++abi
endif

all: $(ICG)

$(ICG): $(OBJECTS) $(UDUNITS_OBJS)
	$(CXX) -o $@ $(OBJECTS) $(UDUNITS_OBJS) $(CLANGLIBS) $(LLVMLDFLAGS)

# Only FieldDescription.cpp includes the units conversion header.
$(OBJ_DIR)/FieldDescription.o : CXXFLAGS += -I$(TRICK_HOME)/include
$(OBJ_DIR)/HeaderSearchDirs.o : CXXFLAGS += -DLLVM_HOME=\"${LLVM_HOME}\"
$(OBJ_DIR)/main.o : CXXFLAGS += $(UNITS_CONV_INCLUDE) -DTRICK_VERSION=\"${TRICK_VERSION}\"

$(OBJECTS): $(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(UDUNITS_OBJS): $(OBJ_DIR)/%.o : ${TRICK_HOME}/trick_source/sim_services/UdUnits/%.cpp
	$(CXX) -c $< -o $@

$(OBJECTS) $(UDUNITS_OBJS) : | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

clean:
	rm -rf $(OBJ_DIR) $(ICG)


# The config_${HOST_TYPE}.mk file provides LLVM_HOME
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

RTTIFLAG := -fno-rtti
LLVM_VERSION := $(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --version)
ifneq ($(LLVM_HOME),)
CC := $(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --bindir)/clang
CXX := $(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --bindir)/clang++
CXXFLAGS := $(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --cxxflags) -I$(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --includedir) $(RTTIFLAG) -g -O0
LLVMLIBDIR := -L$(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --libdir)
endif
LLVMLDFLAGS := $(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --ldflags)
OBJ_DIR :=  object_$(TRICK_HOST_CPU)

CLANG_VERSION := $(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --version)
CLANG_MINOR_GTEQ5 := $(shell expr `$(LLVM_HOME)/bin/$(LLVM_CONFIG) --version | cut -f2 -d. ` \>= 5 )
CLANG_LIBDIR := $(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --libdir)

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

ifeq ($(CLANG_MINOR_GTEQ5),1)
LLVMLDFLAGS += $(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --system-libs)
endif

ifeq ($(CLANG_VERSION),3.1)
CLANGLIBS = \
    -lclangFrontend \
    -lclangDriver \
    -lclangSerialization \
    -lclangParse \
    -lclangSema \
    -lclangAnalysis \
    -lclangRewrite \
    -lclangEdit \
    -lclangAST \
    -lclangLex \
    -lclangBasic \
    -lLLVMMC \
    -lLLVMSupport
else
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
    $(shell $(LLVM_HOME)/bin/$(LLVM_CONFIG) --libs)
endif

all: $(ICG) lib/clang/$(LLVM_VERSION)

$(ICG): $(OBJECTS) $(UNITS_CONV_OBJ)
	$(CXX) -o $@ $(OBJECTS) $(UNITS_CONV_OBJ) $(LLVMLIBDIR) $(CLANGLIBS) $(LLVMLDFLAGS)

lib/clang:
	mkdir -p $@

lib/clang/$(LLVM_VERSION): | lib/clang
	cp -r $(CLANG_LIBDIR)/clang/$(LLVM_VERSION) lib/clang

# Only FieldDescription.cpp includes the units conversion header.
$(OBJ_DIR)/FieldDescription.o : CXXFLAGS += $(UNITS_CONV_INCLUDE)
$(OBJ_DIR)/main.o : CXXFLAGS += $(UNITS_CONV_INCLUDE)

$(OBJECTS): $(OBJ_DIR)/%.o : %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(UNITS_CONV_OBJ): $(UNITS_CONV_SRC)
	$(CC) $(UNITS_CONV_INCLUDE) -c $< -o $@

$(OBJECTS) $(UNITS_CONV_OBJ) : | $(OBJ_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# if an LLVM_HOME is not specified, then ICG cannot be rebuilt... don't delete it
clean:
ifneq ($(LLVM_HOME),)
	-rm -rf $(OBJ_DIR) $(ICG) lib
endif

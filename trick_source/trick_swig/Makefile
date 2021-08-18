TRICK_HOME := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))../..)
include ${TRICK_HOME}/share/trick/makefiles/Makefile.common

ifneq ($(HDF5),)
TRICK_CXXFLAGS += -DHDF5
endif

TRICK_LIB = $(TRICK_LIB_DIR)/libtrick_pyip.a

OBJECT_FILES = \
    $(OBJ_DIR)/PrimitiveAttributesMap.o \
    $(OBJ_DIR)/swig_convert_units.o \
    $(OBJ_DIR)/swig_global_vars.o

SWIG_OBJECT_FILES = \
    $(OBJ_DIR)/sim_services_wrap.o \
    $(OBJ_DIR)/swig_double_wrap.o \
    $(OBJ_DIR)/swig_int_wrap.o \
    $(OBJ_DIR)/swig_ref_wrap.o

PY_FILES = \
    ${TRICK_HOME}/share/trick/swig/sim_services.py \
    ${TRICK_HOME}/share/trick/swig/swig_double.py \
    ${TRICK_HOME}/share/trick/swig/swig_int.py \
    ${TRICK_HOME}/share/trick/swig/swig_ref.py

SWIG_SRC_FILES = $(addprefix swig_${TRICK_HOST_CPU}/, $(notdir $(subst .o,.cpp,$(SWIG_OBJECT_FILES))))
TRICK_CXXFLAGS += $(PYTHON_INCLUDES) -Wno-redundant-decls -Wno-shadow -Wno-unused-parameter -Wno-missing-field-initializers

ifeq ($(IS_CC_CLANG), 1)
TRICK_CXXFLAGS += -Wno-self-assign -Wno-sometimes-uninitialized
endif

TEST_DIR = swig_${TRICK_HOST_CPU}_test

ifneq ($(HDF5),)
SWIG_DEFS += -DHDF5
ifneq ($(HDF5),/usr)
TRICK_CXXFLAGS += -I$(HDF5)/include
endif
endif

ifneq ($(GSL_HOME),)
SWIG_DEFS += -D_HAVE_GSL
endif

ifeq ($(USE_ER7_UTILS), 1)
SWIG_DEFS += -DUSE_ER7_UTILS_INTEGRATORS
endif

ifeq ($(USE_ER7_UTILS_CHECKPOINTHELPER), 1)
SWIG_DEFS += -DUSE_ER7_UTILS_CHECKPOINTHELPER
endif

ifeq ($(TRICK_CIVET), 1)
SWIG_DEFS += -DUSE_CIVET
endif

default: $(SWIG_OBJECT_FILES) $(TRICK_LIB) $(TEST_DIR)

trick: $(SWIG_OBJECT_FILES) $(OBJECT_FILES) $(TEST_DIR)

$(OBJ_DIR):
	mkdir $@

$(TEST_DIR): swig_${TRICK_HOST_CPU}
	ln -s -f $< $@

$(SWIG_OBJECT_FILES): | $(OBJ_DIR)

$(SWIG_SRC_FILES): | swig_${TRICK_HOST_CPU}

swig_${TRICK_HOST_CPU}:
	mkdir $@

$(SWIG_OBJECT_FILES): $(OBJ_DIR)/%.o : swig_${TRICK_HOST_CPU}/%.cpp
	$(TRICK_CXX) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) -c $< -o $@

$(OBJECT_FILES): $(OBJ_DIR)/%.o : %.cpp
	$(TRICK_CXX) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) -c $< -o $@

$(SWIG_SRC_FILES): swig_${TRICK_HOST_CPU}/%_wrap.cpp : %.i
	 $(SWIG) $(SWIG_DEFS) -I${TRICK_HOME}/trick_source -I${TRICK_HOME}/include -I${TRICK_HOME} -c++ -python -includeall -ignoremissing -w201,362,389,451 -o $@ -outdir ${TRICK_HOME}/share/trick/swig $<

$(TRICK_LIB) : $(SWIG_OBJECT_FILES) $(OBJECT_FILES)
	ar crs $@ $?

real_clean: clean

clean: clean_python
	$(RM) -rf $(OBJ_DIR) $(TEST_DIR) $(PY_FILES)


clean_python:
	rm -rf swig_${TRICK_HOST_CPU} sim_services.py swig_double.py swig_int.py swig_ref.py

real_clean: clean

# some depends
swig_int_wrap.cpp: swig_int.cpp
swig_double_wrap.cpp: swig_double.cpp
swig_ref_wrap.cpp: swig_ref.cpp


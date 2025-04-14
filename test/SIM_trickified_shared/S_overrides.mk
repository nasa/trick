THIS_IS_MY_LOCAL_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

#Normally would just include S_overrides_trickify.mk like below. Due to the weird order of operations in this test, the file won't be generated before this include statement. I've copied the contents of the file instead.
#include $(THIS_IS_MY_LOCAL_DIR)/trickified_project/trickified/S_overrides_trickify.mk

BUILD_TYPE := so
UNAME := $(shell uname)
ifeq ($(UNAME),Darwin)
	BUILD_TYPE := dylib
endif

TRICK_LDFLAGS += $(THIS_IS_MY_LOCAL_DIR)/trickified_project/trickified/trickified_myproject.$(BUILD_TYPE)
TRICK_EXT_LIB_DIRS += :$(THIS_IS_MY_LOCAL_DIR)/trickified_project/include_bar:$(THIS_IS_MY_LOCAL_DIR)/trickified_project/include_foo
TRICK_PYTHON_PATH += :$(THIS_IS_MY_LOCAL_DIR)/trickified_project/trickified/python
TRICK_SWIG_FLAGS += -I$(THIS_IS_MY_LOCAL_DIR)/trickified_project/trickified/

TRICK_CXXFLAGS += -I$(THIS_IS_MY_LOCAL_DIR)/models
include $(CURDIR)/trickified_project/trickified/myproject.mk

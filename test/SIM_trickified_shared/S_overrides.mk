LOCAL_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

#Normally would just include S_overrides_trickify.mk like below. Due to the weird order of operations in this test, the file won't be generated before this include statement. I've copied the contents of the file instead.
#include $(LOCAL_DIR)/trickified_project/trickified/S_overrides_trickify.mk
TRICK_LDFLAGS += $(LOCAL_DIR)/trickified_project/trickified/trickified_myproject.so
TRICK_EXT_LIB_DIRS += $(LOCAL_DIR)/trickified_project/include_bar:$(LOCAL_DIR)/trickified_project/include_foo
TRICK_PYTHON_PATH += $(LOCAL_DIR)/trickified_project/trickified/python
TRICK_SWIG_FLAGS += -I$(LOCAL_DIR)/trickified_project/trickified/

TRICK_CXXFLAGS += -I$(LOCAL_DIR)/models
include $(CURDIR)/trickified_project/trickified/myproject.mk

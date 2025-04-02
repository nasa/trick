LOCAL_DIR := $(abspath $(dir $(lastword $(MAKEFILE_LIST))))

include trickified_project/trickified/S_overrides_trickify.mk

TRICK_CXXFLAGS += -I$(LOCAL_DIR)/models
include $(CURDIR)/trickified_project/trickified/myproject.mk

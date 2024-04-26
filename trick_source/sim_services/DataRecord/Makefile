include $(dir $(lastword $(MAKEFILE_LIST)))../../../share/trick/makefiles/Makefile.common
include ${TRICK_HOME}/share/trick/makefiles/Makefile.tricklib
-include Makefile_deps

TRICK_CXXFLAGS +=

ifneq ($(HDF5),)
ifneq ($(HDF5),/usr)
$(OBJ_DIR)/DRHDF5.o: TRICK_CXXFLAGS += -I$(HDF5)/include
$(OBJ_DIR)/io_DRHDF5.o: TRICK_CXXFLAGS += -I$(HDF5)/include
endif
TRICK_CXXFLAGS += -DHDF5
endif

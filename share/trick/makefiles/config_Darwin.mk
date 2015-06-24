
INSTALL = /usr/bin/install

SHARED_LIB_OPT := -dynamiclib -Wl,-undefined,dynamic_lookup

TRICK_LIB_DIR  := ${TRICK_HOME}/lib
TRICK_LDFLAGS +=
LD_WHOLE_ARCHIVE := -Wl,-all_load
LD_NO_WHOLE_ARCHIVE :=
TRICK_EXEC_LINK_LIBS += -lm -lpthread -ldl -lz $(PYTHON_LIB)


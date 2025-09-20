
TRICK_CFLAGS += -I${TRICK_HOME}/trick_models -Imodels
TRICK_CXXFLAGS += -I${TRICK_HOME}/trick_models -Imodels
TRICK_EXEC_LINK_LIBS := $(filter-out -lintl,$(TRICK_EXEC_LINK_LIBS))

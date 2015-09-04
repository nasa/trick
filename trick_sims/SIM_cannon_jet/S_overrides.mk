ifndef TRICK_HOME
    $(error TRICK_HOME environment variable is not set. Please set it and retry.)
endif

TRICK_CFLAGS += -I${TRICK_HOME}/trick_models
TRICK_CXXFLAGS += -I${TRICK_HOME}/trick_models

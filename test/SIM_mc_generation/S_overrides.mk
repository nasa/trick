TRICK_CFLAGS += -g -Wall -Wextra
TRICK_CXXFLAGS += -g -Wall -Wextra
# We can't yet make warnings to be errors on MacOS, because
# MACOS deprecates and warns about sprintf. But SWIG
# still generates code containing sprintf..
ifneq ($(TRICK_HOST_TYPE), Darwin)
TRICK_CXXFLAGS += -Werror -Wno-stringop-truncation
endif

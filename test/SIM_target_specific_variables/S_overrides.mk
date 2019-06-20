MODELS :=$(CURDIR)/models

# Warn about comment markers /* within comments and make them errors, causing
# the build to fail.
TRICK_CXXFLAGS += -I$(MODELS) -Wcomment -Werror

PATHS := $(MODELS)/many $(MODELS)/nested

# The zero directory contains no files, so FIND_MODEL_OBJECTS should return an
# empty list, resulting in a rule that is never run. If it is, the invalid
# "not_a_flag" should cause the build to fail.
$(call FIND_MODEL_OBJECTS, $(MODELS)/zero): TRICK_CXXFLAGS += not_a_flag

# By turning off warnings about comment markers within comments in all the
# offending files, this sim should build without error.
$(call FIND_MODEL_OBJECTS, $(MODELS)/one/1 $(PATHS)): TRICK_CXXFLAGS += -Wno-comment

include $(PARENT_DIR)Makefile.common

TRICKIFY_FAKE_DEP_PATHS := ""
ifneq ($(wildcard build/),build/trickify/swig/fake_deps_paths)
	TRICKIFY_FAKE_DEP_PATHS := $(shell cat build/trickify/swig/fake_deps_paths)
endif

compile_swig:
	$(call ECHO_AND_LOG,$(TRICK_CXX) $(TRICKIFY_FAKE_DEP_PATHS) $(TRICK_CXXFLAGS) $(TRICK_SYSTEM_CXXFLAGS) $(PYTHON_INCLUDES) $(TRICK_SWIG_CFLAGS) $(TRICK_SYSTEM_SWIG_CFLAGS) -Wno-unused-parameter -Wno-shadow -c -o $(ARG1) $(ARG2))

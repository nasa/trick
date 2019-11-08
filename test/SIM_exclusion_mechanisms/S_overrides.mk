MODELS := $(CURDIR)/models
TRICK_CXXFLAGS += -I$(MODELS)

TRICK_EXCLUDE += :$(MODELS)/Exclude.hh
TRICK_EXCLUDE += :$(MODELS)/exclude
TRICK_EXCLUDE += :$(MODELS)/symlinks/exclude_link_path/links/exclude
TRICK_EXCLUDE += :$(MODELS)/symlinks/exclude_real_path/real/exclude

TRICK_ICG_EXCLUDE += :$(MODELS)/IcgExclude.hh
TRICK_ICG_EXCLUDE += :$(MODELS)/icg_exclude
TRICK_ICG_EXCLUDE += :$(MODELS)/symlinks/exclude_link_path/links/icg_exclude
TRICK_ICG_EXCLUDE += :$(MODELS)/symlinks/exclude_real_path/real/icg_exclude

TRICK_SWIG_EXCLUDE += :$(MODELS)/SwigExclude.hh
TRICK_SWIG_EXCLUDE += :$(MODELS)/swig_exclude
TRICK_SWIG_EXCLUDE += :$(MODELS)/symlinks/exclude_link_path/links/swig_exclude
TRICK_SWIG_EXCLUDE += :$(MODELS)/symlinks/exclude_real_path/real/swig_exclude

TRICK_EXT_LIB_DIRS += :$(MODELS)/ExtLibDir.hh
TRICK_EXT_LIB_DIRS += :$(MODELS)/ExtLibDir.cpp
TRICK_EXT_LIB_DIRS += :$(MODELS)/ext_lib_dir
TRICK_EXT_LIB_DIRS += :$(MODELS)/symlinks/exclude_link_path/links/ext_lib_dir
TRICK_EXT_LIB_DIRS += :$(MODELS)/symlinks/exclude_real_path/real/ext_lib_dir

TRICKIFIED := $(MODELS)/trickified/trickified.o
TRICK_PYTHON_PATH += :$(MODELS)/trickified/python
TRICK_SWIG_FLAGS += -I$(MODELS)/trickified
TRICK_LDFLAGS += $(TRICKIFIED)
$(SWIG_SRC): $(TRICKIFIED)

$(TRICKIFIED):
	@$(MAKE) -s -C $(MODELS)/trickified

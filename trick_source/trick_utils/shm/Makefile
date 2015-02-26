
# $Id: Makefile 2600 2012-09-18 23:05:53Z wwoodard $

ifdef TRICK_HOME
ifneq ($(STAND_ALONE), 1)

-include ${TRICK_HOME}/makefiles/Makefile.common
-include ${TRICK_HOME}/makefiles/Makefile.tricklib

-include Makefile_deps

SEPARATE_LIB = $(TRICK_LIB_DIR)/libtrick_shm.a

default: $(SEPARATE_LIB)
trick: $(SEPARATE_LIB)

stand_alone:
	@ $(MAKE) STAND_ALONE=1

clean_stand_alone:
	@ $(MAKE) STAND_ALONE=1 clean

$(SEPARATE_LIB) : $(OBJECT_FILES) $(IO_OBJS) | $(TRICK_LIB_DIR)
	ar cr $@ $(OBJECT_FILES) $(IO_OBJS)

else

include makefile.stand_alone

endif
else

include makefile.stand_alone

endif


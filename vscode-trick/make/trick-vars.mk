# Evaluates the same flag variables bin/trick-CP would see for a given sim
# directory, without actually running CP/ICG/compiling anything. Invoked by
# the extension as:
#
#   make -s -C <simRoot> -f <this file> print-TRICK_CFLAGS print-TRICK_CXXFLAGS ...
#
# with TRICK_HOME set in the environment. Mirrors the include order used by
# bin/trick-CP (see its "-include S_overrides.mk" near the end of that file).

ifndef TRICK_HOME
$(error TRICK_HOME is not set)
endif

include $(TRICK_HOME)/share/trick/makefiles/Makefile.common

-include S_overrides.mk
-include trickify.mk
-include S_post.mk

print-%:
	@echo '$($*)'

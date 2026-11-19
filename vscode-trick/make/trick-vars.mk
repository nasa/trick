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

# Tagged rather than a bare `@echo '$($*)'`, because a sim's own
# S_overrides.mk (or something it includes, e.g. a wrapped package's
# makefile) can perfectly legally emit its own $(info .../$(warning ... text
# to stdout while make is still parsing - before any print-% target's recipe
# even runs. The caller (simConfig.ts's resolveViaMake) can no longer assume
# "line N of stdout is variable N" once that happens, since those extra
# lines shift everything after them - it instead picks each variable's value
# out by this marker, wherever it lands.
print-%:
	@echo 'TRICK_VSCODE_FLAG:$*:$($*)'

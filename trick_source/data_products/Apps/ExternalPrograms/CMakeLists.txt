# DP "external program" plugin .so's — ported from this directory's own
# makefile (the authority). Only dp_test.so and dp_subtract.so are actually
# built by `make all` (OBJECTS0's other targets, e.g. dp_lvlh_attitude.so,
# are real rules but nothing in the makefile's dependency graph reaches
# them — `objects: $(OBJECTS1)` where OBJECTS1 = dp_test.so dp_subtract.so
# only; the fuller OBJECTS0 list is commented out). No stale 2019
# CMakeLists.txt existed for this directory.
add_library(dp_test MODULE dp_test.c)
add_library(dp_subtract MODULE dp_subtract.c)
set_target_properties(dp_test dp_subtract PROPERTIES
    PREFIX ""
    SUFFIX ".so"
)

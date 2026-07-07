# libeqparse.a — ported from this directory's own makefile (the authority).
# eqparse_test.c is a standalone smoke-test executable's source, NOT part of
# the library (makefile: E_C_SRC = $(filter-out eqparse_test.c, $(wildcard
# *.c))) — the stale 2019 CMakeLists.txt included it in the library, which
# is wrong (see CMAKE_MIGRATION_PLAN.md Part A4: the stale files are hints,
# not the authority).
set(DP_EQPARSE_SRC
    eqparse.c
    eqparse_chkvalid.c
    eqparse_error.c
    eqparse_evaluate.c
    eqparse_fillno.c
    eqparse_funcsub.c
    eqparse_math.c
    eqparse_negcheck.c
    eqparse_operatorcheck.c
    eqparse_postfix.c
    eqparse_stack.c
    eqparse_takeinput.c
)

add_library(dp_eqparse STATIC ${DP_EQPARSE_SRC})
set_target_properties(dp_eqparse PROPERTIES ARCHIVE_OUTPUT_NAME eqparse)

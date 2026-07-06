# Phase 3 — CTest wiring.
#
# Two kinds of tests, both run against a staged TRICK_HOME (never the source tree,
# never a make build):
#
#   LABEL unit  — the per-directory gtest suites under trick_source/**/test. Each
#                 has a hand-written Makefile that `include`s Makefile.common; that
#                 file force-computes TRICK_HOME from its own location, but the
#                 assignment is plain (not `override`), so a `make TRICK_HOME=<stage>`
#                 command-line override wins and redirects config_user.mk, the
#                 archives (TRICK_LIB_DIR), headers, and trick-ICG to the stage.
#
#   LABEL sims  — the universal acceptance test: trick-CP builds and runs a real sim
#                 against the stage (test/build_config/run_sim_flow.sh).
#
# A CTest fixture stages the install first (cmake --install into ${TRICK_STAGE_DIR}),
# so `ctest` works after a plain `cmake --build`. The build itself must be complete
# before ctest — ctest does not compile the archives.

include(CTest)

if(NOT BUILD_TESTING)
    return()
endif()

if(NOT DEFINED TRICK_STAGE_DIR)
    set(TRICK_STAGE_DIR ${CMAKE_BINARY_DIR}/stage)
endif()

# ── Setup fixture: install the build tree into the stage prefix.
add_test(NAME stage_install
    COMMAND ${CMAKE_COMMAND} --install ${CMAKE_BINARY_DIR} --prefix ${TRICK_STAGE_DIR}
)
set_tests_properties(stage_install PROPERTIES FIXTURES_SETUP trick_stage)

# ── Unit tests: mirror the top-level Makefile's UNIT_TEST_DIRS (sim_services/*/test
#    and trick_utils/*/test), with the same %Integrator/test filter (Makefile:137).
file(GLOB _tr_unit_test_dirs
    ${CMAKE_SOURCE_DIR}/trick_source/sim_services/*/test
    ${CMAKE_SOURCE_DIR}/trick_source/trick_utils/*/test
)
foreach(_dir ${_tr_unit_test_dirs})
    if(NOT EXISTS ${_dir}/Makefile)
        continue()
    endif()
    if(_dir MATCHES "Integrator/test$")
        continue()   # filtered out of releases by the make build (Makefile:137)
    endif()
    # ut_<parent-dir-name>, e.g. ut_MemoryManager
    get_filename_component(_parent ${_dir} DIRECTORY)
    get_filename_component(_name ${_parent} NAME)
    add_test(NAME ut_${_name}
        COMMAND make -C ${_dir} test TRICK_HOME=${TRICK_STAGE_DIR}
    )
    set_tests_properties(ut_${_name} PROPERTIES
        LABELS unit
        FIXTURES_REQUIRED trick_stage
        TIMEOUT 900
    )
endforeach()

# ── Sim-flow acceptance tests (LABEL sims): trick-CP builds + runs real sims from
#    the stage. One data-recording/checkpoint sim and one STL sim, matching the
#    plan's Phase 3 step 5 ("SIM_cannon_analytic plus one data-recording/STL sim" —
#    cannon_analytic isn't in this checkout; the demo/STL sims exercise the same
#    trick-CP -> ICG -> SWIG -> compile -> run path).
set(_tr_sim_flow_script ${CMAKE_SOURCE_DIR}/test/build_config/run_sim_flow.sh)
function(trick_add_sim_flow_test name sim_dir)
    if(NOT EXISTS ${CMAKE_SOURCE_DIR}/${sim_dir}/S_define)
        return()
    endif()
    add_test(NAME ${name}
        COMMAND ${_tr_sim_flow_script} ${TRICK_STAGE_DIR} ${CMAKE_SOURCE_DIR}/${sim_dir}
    )
    set_tests_properties(${name} PROPERTIES
        LABELS sims
        FIXTURES_REQUIRED trick_stage
        TIMEOUT 1800
    )
endfunction()

trick_add_sim_flow_test(sim_demo_sdefine test/SIM_demo_sdefine)
trick_add_sim_flow_test(sim_stls        test/SIM_stls)

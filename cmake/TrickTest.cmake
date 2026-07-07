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
#    and trick_utils/*/test), with the same %Integrator/test filter — conditional on
#    USE_ER7_UTILS, exactly like Makefile:136-138 (`ifeq ($(USE_ER7_UTILS), 0)`), not
#    unconditional. This checkout defaults TRICK_USE_ER7_UTILS=ON, so Integrator/test
#    is normally INCLUDED, matching `make unit_test`'s real dir count.
file(GLOB _tr_unit_test_dirs
    ${CMAKE_SOURCE_DIR}/trick_source/sim_services/*/test
    ${CMAKE_SOURCE_DIR}/trick_source/trick_utils/*/test
)
foreach(_dir ${_tr_unit_test_dirs})
    if(NOT EXISTS ${_dir}/Makefile)
        continue()
    endif()
    if(NOT TRICK_USE_ER7_UTILS AND _dir MATCHES "Integrator/test$")
        continue()   # filtered out when ER7 utils are off (Makefile:136-138)
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

# ── DPX (data_products) unit tests: same LABEL unit as above, but its own
#    makefile needs extra overrides (see that makefile's header comment)
#    because CMake builds each data_products library into its own target
#    output directory instead of one shared lib_<cpu>/ dir. TRICK_SRC_HOME
#    keeps headers resolving against the real source tree even though
#    TRICK_HOME below is the staged install.
if(TRICK_BUILD_DP)
    set(_tr_dpx_unit_test_dir ${CMAKE_SOURCE_DIR}/trick_source/data_products/DPX/test/unit_test)
    if(EXISTS ${_tr_dpx_unit_test_dir}/makefile)
        add_test(NAME ut_DataProducts
            COMMAND make -C ${_tr_dpx_unit_test_dir} test
                TRICK_HOME=${TRICK_STAGE_DIR}
                TRICK_SRC_HOME=${CMAKE_SOURCE_DIR}
                LIB_LOG_DIR=${CMAKE_BINARY_DIR}/trick_source/data_products/Log
                LIB_VAR_DIR=${CMAKE_BINARY_DIR}/trick_source/data_products/Var
                LIB_EQPARSE_DIR=${CMAKE_BINARY_DIR}/trick_source/data_products/EQParse
                LIB_UNITS_DIR=${CMAKE_BINARY_DIR}/trick_source/data_products/units
                LIB_DPM_DIR=${CMAKE_BINARY_DIR}/trick_source/data_products/DPX/DPM
                LIB_DPC_DIR=${CMAKE_BINARY_DIR}/trick_source/data_products/DPX/DPC
        )
        set_tests_properties(ut_DataProducts PROPERTIES
            LABELS unit
            FIXTURES_REQUIRED trick_stage
            TIMEOUT 900
        )
    endif()
endif()

# ── Java unit tests (JUnit, via `mvn test`). Unlike the C++ gtest suites, Maven
#    resolves everything relative to trick_source/java itself, so no TRICK_HOME
#    path-shape juggling is needed; -Dcmake=false matches the top-level
#    Makefile's `test:` target (Makefile in trick_source/java) and writes
#    surefire reports/compiled test classes under libexec/trick/java/build in
#    the source tree — test output, not a product-build write (D3 is about the
#    product build, same class of exception already noted for the C++ gtest
#    suites' own **/test artifacts).
if(TRICK_USE_JAVA)
    add_test(NAME ut_java
        COMMAND ${MVN_EXECUTABLE} test -Dcmake=false -Dmaven.wagon.http.retryHandler.count=15
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/trick_source/java
    )
    set_tests_properties(ut_java PROPERTIES
        LABELS unit
        TIMEOUT 900
    )
endif()

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

# ── Full sim suite (LABEL sims_full): the same trickops.py/test_sims.yml suite
#    `make sim_test` runs (test_overrides.mk), covering all ~52 sims in
#    test_sims.yml — the two lightweight sim_demo_sdefine/sim_stls tests above are
#    a fast subset/smoke-check of the same trick-CP path, not a substitute for
#    this. trickops.py's --trick_dir (added alongside this CTest wiring) points
#    build_cmd at <stage>/bin/trick-CP, which self-derives TRICK_HOME from its own
#    location (bin/trick-CP:13, dirname($trick_bin)) — no TRICK_HOME env needed.
#    --trick_top_level stays the source tree, since the test/SIM_* sim
#    directories test_sims.yml references only exist there, not in the stage.
if(EXISTS ${CMAKE_SOURCE_DIR}/test_sims.yml)
    find_package(Python3 COMPONENTS Interpreter)
    add_test(NAME sims_full
        COMMAND ${Python3_EXECUTABLE} ${CMAKE_SOURCE_DIR}/trickops.py
            --trick_top_level ${CMAKE_SOURCE_DIR}
            --trick_dir ${TRICK_STAGE_DIR}
            --config_file test_sims.yml
            --quiet
    )
    set_tests_properties(sims_full PROPERTIES
        LABELS sims_full
        FIXTURES_REQUIRED trick_stage
        TIMEOUT 3600
    )
endif()

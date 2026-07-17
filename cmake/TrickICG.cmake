# io_src generation for Trick's own core build (Part C, Phase 2, step 3).
#
# Requires trick-ICG (add_subdirectory(trick_source/codegen/Interface_Code_Gen)
# already processed) plus the detection variables from Phase 1.
#
# ICG's output-file placement is dynamic — it discovers headers transitively
# via clang's preprocessor, not just the direct #includes in files_to_ICG.hh
# (confirmed: trick_source/er7_utils/**/io_src/ contain io_ files for headers
# nowhere directly #included). CMake's add_library() needs a static SOURCES
# list at configure time, so this uses the two-pass pattern: a build-time
# custom command runs ICG and writes a small generated .cmake file
# enumerating the real output. CMAKE_CONFIGURE_DEPENDS is set on that file
# for cache-invalidation on later header changes, but CMake only honors
# CONFIGURE_DEPENDS entries that already exist at the *configure* that
# registers them — on a truly clean tree the file doesn't exist yet, so nothing
# auto-triggers a reconfigure once the build-time custom command creates it.
# So on a clean tree this is a three-step bootstrap:
#   cmake -S . -B build
#   cmake --build build --target trick_io_src_gen   # runs ICG, writes the list
#   cmake -S . -B build && cmake --build build       # reconfigure picks up the
#                                                     # real list, then builds
# Once io_src exists, ordinary `cmake --build build` reconfigures/rebuilds
# incrementally as normal (headers touch files_to_ICG.hh's mtime via
# CMAKE_CONFIGURE_DEPENDS below, which DOES work once the file is on disk).

set(TRICK_IO_SRC_DIR ${CMAKE_BINARY_DIR}/io_src)
set(TRICK_IO_SRC_LIST_FILE ${TRICK_IO_SRC_DIR}/.generated_sources.cmake)

# Conservative net (R7): a glob of every header ICG might transitively pull
# in. This does two distinct jobs, both needed:
#  - Passed to the custom command's DEPENDS below, so a *content* edit to any
#    header already in this set (e.g. adding a checkpointable member to
#    include/trick/Executive.hh) touches its mtime and makes the *next*
#    `cmake --build` (no reconfigure needed) re-run trick-ICG and regenerate
#    the stale io_*.cpp — this is the half that actually matters day to day.
#  - CONFIGURE_DEPENDS additionally forces a reconfigure when the *set* of
#    matching files changes (a header added/removed), so newly-created
#    headers get picked up too.
# It does not need to be an exact dependency list — same tradeoff as ICG's
# own conservative header-set discovery — but it must actually be used
# somewhere to do anything at all.
file(GLOB_RECURSE _tr_all_trick_headers CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/include/trick/*.h
    ${CMAKE_SOURCE_DIR}/include/trick/*.hh
    ${CMAKE_SOURCE_DIR}/trick_source/er7_utils/*.hh
)

# ── Flags mirroring TRICK_SYSTEM_CXXFLAGS / TRICK_CXXFLAGS (Makefile.common:98-197) ──
set(_tr_icg_flags
    -I${CMAKE_SOURCE_DIR}/trick_source
    -I${CMAKE_SOURCE_DIR}/include
    -I${CMAKE_SOURCE_DIR}/include/trick/compat
    -DTRICK_VER=${TRICK_MAJOR}
    -DTRICK_MINOR=${TRICK_MINOR}
    -fpic
    ${UDUNITS_INCLUDES}
    -std=c++17
)
if(TRICK_USE_ER7_UTILS)
    list(APPEND _tr_icg_flags -DUSE_ER7_UTILS_INTEGRATORS)
    if(EXISTS ${CMAKE_SOURCE_DIR}/trick_source/er7_utils/CheckpointHelper)
        list(APPEND _tr_icg_flags -DUSE_ER7_UTILS_CHECKPOINTHELPER)
    endif()
endif()
if(TRICK_FORCE_32BIT)
    list(APPEND _tr_icg_flags -m32)
endif()
# GSL: Makefile.common:147-159 puts -D_HAVE_GSL on TRICK_SYSTEM_CXXFLAGS
# whenever GSL_HOME is set (the make ICG pass inherits it), and only adds the
# -I when GSL_HOME is neither /usr nor /usr/local (both already on the
# compiler's default include path).
if(GSL_HOME)
    list(APPEND _tr_icg_flags -D_HAVE_GSL)
    if(NOT GSL_HOME STREQUAL "/usr" AND NOT GSL_HOME STREQUAL "/usr/local")
        list(APPEND _tr_icg_flags -I${GSL_HOME}/include)
    endif()
endif()
# Gate on USE_CIVETWEB/CIVETWEB_HOME (what TrickPrograms.cmake resolved
# detection to), not the raw TRICK_CIVETWEB_HOME cache option: civetweb.h at
# /usr auto-detects to USE_CIVETWEB=1 with TRICK_CIVETWEB_HOME unset, and the
# make build's ICG pass gets -DUSE_CIVETWEB via TRICK_SYSTEM_CXXFLAGS in that
# case too (Makefile.common:162-167) — files_to_ICG.hh has an
# #ifdef USE_CIVETWEB block, so a mismatch silently drops the civetweb
# io_*.cpp from libtrick.a.
if(USE_CIVETWEB)
    list(APPEND _tr_icg_flags -I${CIVETWEB_HOME}/include -DUSE_CIVETWEB)
endif()

file(MAKE_DIRECTORY ${TRICK_IO_SRC_DIR})

# Makefile.common:167 also adds CIVETWEB_HOME to TRICK_SYSTEM_ICG_EXCLUDE
# (exported at :72), keeping ICG out of civetweb's own headers.
set(_tr_icg_env TRICK_HOME=${CMAKE_SOURCE_DIR} TRICK_EXCLUDE=:${UDUNITS_EXCLUDE})
if(USE_CIVETWEB)
    list(APPEND _tr_icg_env TRICK_SYSTEM_ICG_EXCLUDE=${CIVETWEB_HOME})
endif()

add_custom_command(
    OUTPUT ${TRICK_IO_SRC_LIST_FILE}
    COMMAND ${CMAKE_COMMAND} -E env ${_tr_icg_env}
        $<TARGET_FILE:trick-ICG> -force -sim_services -m ${_tr_icg_flags}
        -o ${TRICK_IO_SRC_DIR}
        ${CMAKE_SOURCE_DIR}/include/trick/files_to_ICG.hh
    COMMAND ${CMAKE_COMMAND}
        -DIO_SRC_DIR=${TRICK_IO_SRC_DIR}
        -DER7_UTILS_SRC_DIR=${CMAKE_SOURCE_DIR}/trick_source/er7_utils
        -DOUTPUT_FILE=${TRICK_IO_SRC_LIST_FILE}
        -P ${CMAKE_SOURCE_DIR}/cmake/scripts/GenerateIOSrcList.cmake
    WORKING_DIRECTORY ${TRICK_IO_SRC_DIR}
    DEPENDS trick-ICG ${CMAKE_SOURCE_DIR}/include/trick/files_to_ICG.hh ${_tr_all_trick_headers}
    COMMENT "Running trick-ICG over Trick core headers (-sim_services)"
    VERBATIM
)
add_custom_target(trick_io_src_gen DEPENDS ${TRICK_IO_SRC_LIST_FILE})

if(EXISTS ${TRICK_IO_SRC_LIST_FILE})
    include(${TRICK_IO_SRC_LIST_FILE})
else()
    set(TRICK_IO_SOURCES "")
    set(ER7_IO_SOURCES "")
    message(WARNING
        "io_src has not been generated yet. Run "
        "`cmake --build ${CMAKE_BINARY_DIR} --target trick_io_src_gen`, then "
        "re-run `cmake -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}` and "
        "`cmake --build ${CMAKE_BINARY_DIR}` again so libtrick.a/"
        "liber7_utils.a pick up the generated sources (see cmake/TrickICG.cmake).")
endif()
# Re-run configure automatically once trick_io_src_gen produces a fresh list.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS ${TRICK_IO_SRC_LIST_FILE})

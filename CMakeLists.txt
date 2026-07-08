# Range form: 3.20 is the supported floor (oldest distro CMake we target); the
# upper bound opts every policy introduced through 4.3 into its NEW behavior
# when built with a CMake that new, without raising the floor contributors need.
cmake_minimum_required(VERSION 3.20...4.3)

# Version lives in share/trick/trick_ver.txt — the single source of truth
# also read by bin/trick-version at sim-build time (Makefile.common:28-30).
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/share/trick/trick_ver.txt TR_VER_TXT)
string(REGEX MATCH "current_version *= *\"([0-9]+)\\.([0-9]+)\\.([0-9]+)([^\"]*)\"" _ "${TR_VER_TXT}")
set(TRICK_MAJOR ${CMAKE_MATCH_1})
set(TRICK_MINOR ${CMAKE_MATCH_2})
set(TRICK_TINY ${CMAKE_MATCH_3})
set(TRICK_PRERELEASE ${CMAKE_MATCH_4})

# Mirror AC_PROG_CC/AC_PROG_CXX's default search order (gcc/cc, g++/c++) so
# CMAKE_C_COMPILER/CMAKE_CXX_COMPILER matches what ./configure would pick.
# Compiler selection must happen before project() enables the languages; it
# still honors the CC/CXX environment variables exactly like autoconf does.
if(NOT DEFINED ENV{CC} AND NOT CMAKE_C_COMPILER)
    find_program(TR_C_COMPILER NAMES gcc cc)
    if(TR_C_COMPILER)
        set(CMAKE_C_COMPILER "${TR_C_COMPILER}")
    endif()
endif()
if(NOT DEFINED ENV{CXX} AND NOT CMAKE_CXX_COMPILER)
    find_program(TR_CXX_COMPILER NAMES g++ c++)
    if(TR_CXX_COMPILER)
        set(CMAKE_CXX_COMPILER "${TR_CXX_COMPILER}")
    endif()
endif()

project(trick VERSION ${TRICK_MAJOR}.${TRICK_MINOR}.${TRICK_TINY} LANGUAGES C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

# ──────────────────────────────────────────────────────────────────────────
# Options (Part C, Phase 1, step 1 of CMAKE_MIGRATION_PLAN.md)
# ──────────────────────────────────────────────────────────────────────────
option(TRICK_FORCE_32BIT "Force Trick and sims to compile 32bit" OFF)
option(TRICK_OFFLINE "Compile Trick in offline mode (requires trick-offline/ in the source tree)" OFF)
option(TRICK_USE_JAVA "Build the Trick Java GUIs" ON)
option(TRICK_BUILD_DP "Build the X11/Motif data_products tools" ON)
set(TRICK_CIVETWEB_HOME "" CACHE PATH "CivetWeb root directory (empty = auto-detect/disabled)")
set(HDF5_HOME "" CACHE PATH "HDF5 root directory (empty = auto-detect)")
set(GSL_HOME "" CACHE PATH "GSL root directory (empty = auto-detect)")
set(GTEST_HOME "" CACHE PATH "GoogleTest root directory (empty = auto-detect)")
set(LLVM_HOME "" CACHE PATH "LLVM root directory (empty = auto-detect)")
set(UDUNITS_HOME "" CACHE PATH "UDUnits2 root directory (empty = auto-detect)")
set(PYTHON_VERSION "" CACHE STRING "Suffix for python search, e.g. 3 looks for python3")

# -m32 propagates through the trick_build_flags INTERFACE target below, NOT
# directory-wide: Makefile.common:119-122 only puts -m32 on TRICK_ICGFLAGS/
# TRICK_SYSTEM_CXXFLAGS/TRICK_SYSTEM_LDFLAGS (flags for sim/library code),
# never on the trick-ICG tool's own compile — a 32-bit ICG can't link the
# 64-bit LLVM archives — and data_products' own -m32 gate
# (TRICK_DP_FORCE_32BIT) is dead upstream, so it never builds 32-bit either.
if(TRICK_FORCE_32BIT)
    set(TRICK_FORCE_32BIT_MK 1)
else()
    set(TRICK_FORCE_32BIT_MK 0)
endif()

if(TRICK_OFFLINE)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/trick-offline")
        message(FATAL_ERROR "Offline mode requires an uncompressed directory named \"trick-offline\" in TRICK_HOME")
    endif()
    set(TRICK_OFFLINE_MK 1)
else()
    set(TRICK_OFFLINE_MK 0)
endif()

# ──────────────────────────────────────────────────────────────────────────
# Detection (no compilation happens in Phase 1 — see A2/D3 in the plan: sims
# consume config_user.mk via the make flow, which is the real product here)
# ──────────────────────────────────────────────────────────────────────────
include(TrickPrograms)
include(FindLLVMClang)
include(TrickClangLibs)
include(TrickPython)
include(FindUDUNITS2)
include(TrickGTest)

# ──────────────────────────────────────────────────────────────────────────
# config_user.mk (D4: golden-file-tested parity with autoconf's output)
# ──────────────────────────────────────────────────────────────────────────
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/share/trick/makefiles/config_user_cmake.mk.in
    ${CMAKE_BINARY_DIR}/share/trick/makefiles/config_user.mk
    @ONLY
)

# ──────────────────────────────────────────────────────────────────────────
# Phase 2 — core native build: ICG, io_src, archives, SWIG
# ──────────────────────────────────────────────────────────────────────────
find_package(Python3 REQUIRED COMPONENTS Interpreter Development.Embed)
find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

# Flags shared by every Trick-core compile target, mirroring
# TRICK_SYSTEM_CXXFLAGS/TRICK_CXXFLAGS (Makefile.common:98-197). HDF5's
# -DHDF5/-I only apply to DataRecord/DRHDF5.cpp in the make build too (that
# file's own Makefile-local override — see sim_services/CMakeLists.txt), so
# it stays per-file here rather than global. GSL is different: Makefile.common
# itself puts -D_HAVE_GSL/-I on TRICK_SYSTEM_CXXFLAGS globally (every compiled
# file inherits it); MonteCarlo/Makefile's own local-looking GSL override is
# actually dead code (gated on a $(HAVE_GSL) variable nothing ever sets), so
# GSL flags belong here, build-wide, not scoped to one file.
add_library(trick_build_flags INTERFACE)
target_compile_features(trick_build_flags INTERFACE cxx_std_17)
# Position-independent code via the CMake property rather than a literal -fpic,
# so the compiler-appropriate flag is emitted. INTERFACE_POSITION_INDEPENDENT_CODE
# propagates POSITION_INDEPENDENT_CODE=ON to every target that links
# trick_build_flags (all the core archives + trick_pyip). Sims compiled by the
# make flow use -fpic; -fPIC is a compatible superset, and archive membership
# (the ar-t parity gate) is unaffected by the flag.
set_target_properties(trick_build_flags PROPERTIES INTERFACE_POSITION_INDEPENDENT_CODE ON)
target_compile_definitions(trick_build_flags INTERFACE
    TRICK_VER=${TRICK_MAJOR}
    TRICK_MINOR=${TRICK_MINOR}
)
target_include_directories(trick_build_flags INTERFACE
    ${CMAKE_SOURCE_DIR}/trick_source
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/include/trick/compat
)
if(UDUNITS_INCLUDES)
    separate_arguments(_tr_udunits_includes_list NATIVE_COMMAND "${UDUNITS_INCLUDES}")
    target_compile_options(trick_build_flags INTERFACE ${_tr_udunits_includes_list})
endif()
if(TRICK_USE_ER7_UTILS)
    target_compile_definitions(trick_build_flags INTERFACE USE_ER7_UTILS_INTEGRATORS)
    if(EXISTS ${CMAKE_SOURCE_DIR}/trick_source/er7_utils/CheckpointHelper)
        target_compile_definitions(trick_build_flags INTERFACE USE_ER7_UTILS_CHECKPOINTHELPER)
    endif()
endif()
if(TRICK_FORCE_32BIT)
    target_compile_options(trick_build_flags INTERFACE -m32)
    target_link_options(trick_build_flags INTERFACE -m32)
endif()
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(trick_build_flags INTERFACE -fcolor-diagnostics)
endif()
# USE_CIVETWEB/CIVETWEB_HOME (not the raw TRICK_CIVETWEB_HOME cache option)
# are what cmake/TrickPrograms.cmake actually resolved detection to — an
# unset TRICK_CIVETWEB_HOME can still auto-detect civetweb.h at /usr and set
# USE_CIVETWEB=1, matching config_user.mk, so this must gate on the same
# variables config_user.mk uses (see A1/tr_civetweb_home.m4).
if(USE_CIVETWEB)
    target_compile_definitions(trick_build_flags INTERFACE USE_CIVETWEB)
    target_include_directories(trick_build_flags INTERFACE ${CIVETWEB_HOME}/include)
endif()
if(GSL_HOME)
    target_compile_definitions(trick_build_flags INTERFACE _HAVE_GSL)
    # Makefile.common:164-177's nested ifneq excludes both /usr and
    # /usr/local (the latter for e.g. Intel-mac Homebrew, whose default
    # prefix is /usr/local rather than Apple Silicon's /opt/homebrew) from
    # needing an explicit -I, since both are already on the compiler's
    # default include search path.
    if(NOT GSL_HOME STREQUAL "/usr" AND NOT GSL_HOME STREQUAL "/usr/local")
        target_include_directories(trick_build_flags INTERFACE ${GSL_HOME}/include)
    endif()
endif()

add_subdirectory(trick_source/codegen/Interface_Code_Gen)
include(TrickICG)

add_subdirectory(trick_source/sim_services)
add_subdirectory(trick_source/trick_utils)
if(TRICK_USE_ER7_UTILS)
    add_subdirectory(trick_source/er7_utils)
endif()
add_subdirectory(trick_source/trick_swig)

# libtrick.a = sim_services + trick_utils objects (minus MemoryManager and
# the trick_utils dirs with their own archives) + the non-er7_utils io_src
# objects (A3).
add_library(trick STATIC
    $<TARGET_OBJECTS:sim_services_objs>
    $<TARGET_OBJECTS:trick_utils_objs>
    ${TRICK_IO_SOURCES}
)
target_link_libraries(trick PUBLIC trick_build_flags)
set_target_properties(trick PROPERTIES ARCHIVE_OUTPUT_NAME trick)
add_dependencies(trick trick_io_src_gen)

# ──────────────────────────────────────────────────────────────────────────
# Phase 4 — optional components: civetweb, Java GUIs, data_products
# ──────────────────────────────────────────────────────────────────────────
if(USE_CIVETWEB)
    add_subdirectory(trick_source/web/CivetServer)
endif()

if(TRICK_USE_JAVA)
    add_subdirectory(trick_source/java)
endif()

if(TRICK_BUILD_DP)
    add_subdirectory(trick_source/data_products)
endif()

# ──────────────────────────────────────────────────────────────────────────
# Phase 3 — install rules, stage target, CTest
# ──────────────────────────────────────────────────────────────────────────
include(TrickInstall)
include(TrickTest)

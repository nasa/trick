cmake_minimum_required(VERSION 3.20)

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

if(TRICK_FORCE_32BIT)
    set(TRICK_FORCE_32BIT_MK 1)
    add_compile_options(-m32)
    add_link_options(-m32)
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
# TRICK_SYSTEM_CXXFLAGS/TRICK_CXXFLAGS (Makefile.common:98-197). A handful of
# these (HDF5/GSL/Python includes, USE_CIVETWEB) only apply to specific
# source files in the make build; applying them build-wide here is a
# deliberate simplification (harmless extra -I/-D flags) rather than
# reproducing every per-file override.
add_library(trick_build_flags INTERFACE)
target_compile_features(trick_build_flags INTERFACE cxx_std_17)
target_compile_options(trick_build_flags INTERFACE -fpic)
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
if(TRICK_CIVETWEB_HOME)
    target_compile_definitions(trick_build_flags INTERFACE USE_CIVETWEB)
    target_include_directories(trick_build_flags INTERFACE ${TRICK_CIVETWEB_HOME}/include)
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

# trick-ICG: real add_executable, ported from this directory's own Makefile
# (the authority per CMAKE_MIGRATION_PLAN.md Part E.3 — the old CMakeLists.txt
# here belonged to the stale 2019 CMake build and is not trusted).
#
# -DEXTERNAL_BUILD changes PrintAttributes' -sim_services behavior only (see
# PrintAttributes.cpp: printSieClass/printSieEnum/createMapFiles), routing the
# combined class/enum map + SIE resource file into -o's output directory
# instead of hardcoded $TRICK_HOME paths. It has no effect on ordinary
# (non -sim_services) per-sim ICG runs, so it is safe to always define — the
# installed trick-ICG behaves identically for user sims either way.

file(GLOB ICG_SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(trick-ICG
    ${ICG_SOURCES}
    ${CMAKE_SOURCE_DIR}/trick_source/sim_services/UdUnits/map_trick_units_to_udunits.cpp
)

# C++ standard by clang major, mirroring this directory's Makefile:16-24.
if(_tr_llvm_version_major GREATER_EQUAL 16)
    set_property(TARGET trick-ICG PROPERTY CXX_STANDARD 17)
elseif(_tr_llvm_version_major GREATER_EQUAL 10)
    set_property(TARGET trick-ICG PROPERTY CXX_STANDARD 14)
else()
    set_property(TARGET trick-ICG PROPERTY CXX_STANDARD 11)
endif()
set_property(TARGET trick-ICG PROPERTY CXX_STANDARD_REQUIRED ON)

target_compile_options(trick-ICG PRIVATE -g -fno-rtti)
target_compile_definitions(trick-ICG PRIVATE
    __STDC_CONSTANT_MACROS
    __STDC_FORMAT_MACROS
    __STDC_LIMIT_MACROS
    EXTERNAL_BUILD
    LIBCLANG_MAJOR=${_tr_llvm_version_major}
)
string(REGEX MATCH "^[0-9]+\\.([0-9]+)" _tr_llvm_minor_match "${LLVM_VERSION_STRING}")
if(CMAKE_MATCH_1)
    target_compile_definitions(trick-ICG PRIVATE LIBCLANG_MINOR=${CMAKE_MATCH_1})
endif()
string(REGEX MATCH "^[0-9]+\\.[0-9]+\\.([0-9]+)" _tr_llvm_patch_match "${LLVM_VERSION_STRING}")
if(CMAKE_MATCH_1)
    target_compile_definitions(trick-ICG PRIVATE LIBCLANG_PATCHLEVEL=${CMAKE_MATCH_1})
endif()

target_include_directories(trick-ICG PRIVATE ${LLVM_INCLUDE_DIR})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_definitions(trick-ICG PRIVATE TRICK_GCC_VERSION="${GCC_VERSION}")
endif()

# Only these two files use these extra per-file defines/includes, matching
# the Makefile's targeted CXXFLAGS overrides (lines 74-76).
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/FieldDescription.cpp PROPERTIES
    COMPILE_OPTIONS "-I${CMAKE_SOURCE_DIR}/include"
)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/HeaderSearchDirs.cpp PROPERTIES
    COMPILE_DEFINITIONS "LLVM_HOME=\"${LLVM_HOME}\""
)
set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/main.cpp PROPERTIES
    COMPILE_DEFINITIONS "TRICK_VERSION=\"${TRICK_MAJOR}.${TRICK_MINOR}.${TRICK_TINY}\""
)

separate_arguments(_tr_icg_clanglibs_list NATIVE_COMMAND "${ICG_CLANGLIBS}")
execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --libs
    OUTPUT_VARIABLE _tr_icg_llvm_libs OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --system-libs
    OUTPUT_VARIABLE _tr_icg_llvm_system_libs OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND ${LLVM_CONFIG_EXECUTABLE} --ldflags
    OUTPUT_VARIABLE _tr_icg_llvm_ldflags OUTPUT_STRIP_TRAILING_WHITESPACE)
separate_arguments(_tr_icg_llvm_libs_list NATIVE_COMMAND "${_tr_icg_llvm_libs}")
separate_arguments(_tr_icg_llvm_ldflags_list NATIVE_COMMAND "${_tr_icg_llvm_ldflags}")

target_link_libraries(trick-ICG PRIVATE ${_tr_icg_clanglibs_list} ${_tr_icg_llvm_ldflags_list} ${_tr_icg_llvm_libs_list} Trick::udunits2)
target_link_directories(trick-ICG PRIVATE ${LLVM_LIB_DIR})

if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    if(NOT LLVM_VERSION_STRING VERSION_LESS "3.5")
        # Fedora adds -ledit as a system lib, but it isn't installed, or required.
        string(REPLACE "-ledit" "" _tr_icg_llvm_system_libs "${_tr_icg_llvm_system_libs}")
        separate_arguments(_tr_icg_llvm_system_libs_list NATIVE_COMMAND "${_tr_icg_llvm_system_libs}")
        target_link_libraries(trick-ICG PRIVATE ${_tr_icg_llvm_system_libs_list})
    endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    separate_arguments(_tr_icg_llvm_system_libs_list NATIVE_COMMAND "${_tr_icg_llvm_system_libs}")
    target_link_libraries(trick-ICG PRIVATE ${_tr_icg_llvm_system_libs_list})
    if(_tr_llvm_version_major GREATER_EQUAL 16)
        target_link_libraries(trick-ICG PRIVATE -lc++abi -lclang-cpp)
    else()
        target_link_libraries(trick-ICG PRIVATE -lc++abi)
    endif()
    # CMake's own automatic rpath handling already embeds an LC_RPATH for
    # LLVM_LIB_DIR here, since we linked against libraries there via
    # target_link_directories; the Makefile's explicit install_name_tool step
    # is therefore usually redundant on this toolchain (but harmless to skip
    # if it fails, e.g. because the rpath is already present).
    add_custom_command(TARGET trick-ICG POST_BUILD
        COMMAND bash -c "install_name_tool -add_rpath '${LLVM_LIB_DIR}' '$<TARGET_FILE:trick-ICG>' 2>/dev/null; true"
        VERBATIM
    )
endif()

# UDUNITS_HOME may be a non-system prefix; keep the build-time rpath to
# libudunits2 in the installed binary (CMake drops build rpath at install by
# default) instead of the makefile's literal -Wl,-rpath.
set_target_properties(trick-ICG PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

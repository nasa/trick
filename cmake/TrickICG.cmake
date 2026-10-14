# Evaluate usage requirements at generation time. HDF5 intentionally remains
# excluded from ICG's historical parsing environment.
include_guard(GLOBAL)

# Keep setup state local. Consumers use trick_add_icg_sources(), not variables
# inherited from the directory that includes this module.
function(_trick_setup_icg)
    set(scripts "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")
    include("${scripts}/TrickICGOutputs.cmake")
    set(root "${PROJECT_BINARY_DIR}/generated/icg")
    set(settings "${PROJECT_BINARY_DIR}/icg/settings.cmake")
    if(CMAKE_CONFIGURATION_TYPES)
        string(APPEND root "/$<CONFIG>")
        set(settings "${PROJECT_BINARY_DIR}/icg/$<CONFIG>/settings.cmake")
    endif()
    set(outputs ${_icg_core_sources} ${_icg_er7_sources}
        sim_services/io_src/extern_init_attr.h sim_services_classes.resource)
    set(byproducts ${outputs})
    list(TRANSFORM byproducts PREPEND "${root}/")
    list(TRANSFORM _icg_core_sources PREPEND "${root}/")
    list(TRANSFORM _icg_er7_sources PREPEND "${root}/")

    # Build-only usage collectors; neither is installed, exported, or for consumers.
    add_library(trick_icg_external INTERFACE)
    target_link_libraries(trick_icg_external INTERFACE UDUNITS2::UDUNITS2)
    if(TRICK_ENABLE_GSL)
        target_link_libraries(trick_icg_external INTERFACE GSL::gsl)
    endif()
    if(TRICK_ENABLE_CIVETWEB)
        target_link_libraries(trick_icg_external INTERFACE civetweb::civetweb-cpp)
    endif()
    add_library(trick_icg_usage INTERFACE)
    target_link_libraries(trick_icg_usage INTERFACE trick_headers trick_icg_external)

    # ICG discovers standard includes via popen(TRICK_CXX), so shell-quote the path.
    string(REPLACE "'" "'\\''" compiler "${CMAKE_CXX_COMPILER}")
    set(compiler "'${compiler}'")
    file(GENERATE OUTPUT "${settings}" CONTENT
"set(icg_local_includes [==[$<TARGET_PROPERTY:trick_headers,INTERFACE_INCLUDE_DIRECTORIES>]==])
set(icg_external_includes [==[$<TARGET_PROPERTY:trick_icg_external,INTERFACE_INCLUDE_DIRECTORIES>]==])
set(icg_definitions [==[$<TARGET_PROPERTY:trick_icg_usage,INTERFACE_COMPILE_DEFINITIONS>]==])
set(icg_implicit_includes [==[${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}]==])
set(icg_source_dir [==[${PROJECT_SOURCE_DIR}]==])
set(icg_binary_dir [==[${PROJECT_BINARY_DIR}]==])
set(icg_compiler [==[${compiler}]==])
set(icg_outputs [==[${outputs}]==])
set(icg_verbose [==[${CMAKE_VERBOSE_MAKEFILE}]==])
" TARGET trick_icg_usage)

    # TODO: have ICG emit a DEPFILE for the headers Clang actually opens, including
    # external/system headers. This glob over-triggers on unrelated local edits
    # and does not directly track external header changes.
    file(GLOB_RECURSE dependencies CONFIGURE_DEPENDS
        "${PROJECT_SOURCE_DIR}/include/*.h" "${PROJECT_SOURCE_DIR}/include/*.hh"
        "${PROJECT_SOURCE_DIR}/include/*.hpp"
        "${PROJECT_SOURCE_DIR}/trick_source/er7_utils/*.hh")
    add_custom_command(
        OUTPUT "${root}.complete"
        BYPRODUCTS ${byproducts}
        COMMAND "${CMAKE_COMMAND}" "-DICG=$<TARGET_FILE:trick-ICG>"
            "-DICG_SETTINGS=${settings}" "-DICG_OUTPUT_ROOT=${root}"
            -P "${scripts}/RunTrickICG.cmake"
        DEPENDS trick-ICG ${dependencies} "${settings}"
            "${scripts}/TrickICGInvoke.cmake" "${scripts}/RunTrickICG.cmake"
        COMMENT "Generating Trick core IO, maps and XML"
        VERBATIM)
    add_custom_target(trick_generation DEPENDS "${root}.complete")
    set_target_properties(trick_generation PROPERTIES
        TRICK_ICG_CORE_SOURCES "${_icg_core_sources}"
        TRICK_ICG_ER7_SOURCES "${_icg_er7_sources}"
        TRICK_ICG_INCLUDE_DIR "${root}/sim_services/io_src")

    if(TRICK_BUILD_TESTS)
        add_test(NAME icg_output_root
            COMMAND "${CMAKE_COMMAND}" "-DICG=$<TARGET_FILE:trick-ICG>"
                "-DICG_SETTINGS=${settings}"
                "-DICG_TEST_ROOT=${PROJECT_BINARY_DIR}/tests/$<CONFIG>/icg-output-root"
                -P "${scripts}/TestTrickICG.cmake")
        set_tests_properties(icg_output_root PROPERTIES LABELS "codegen;slow" TIMEOUT 300)
    endif()
endfunction()

_trick_setup_icg()

# trick_add_icg_sources(<target> CORE|ER7)
# BYPRODUCTS marks the sources GENERATED (CMP0118); no redundant property
# assignment is needed in the consumer's directory.
function(trick_add_icg_sources target component)
    if(NOT component MATCHES "^(CORE|ER7)$")
        message(FATAL_ERROR "trick_add_icg_sources: expected CORE or ER7")
    endif()
    get_target_property(sources trick_generation TRICK_ICG_${component}_SOURCES)
    get_target_property(include_dir trick_generation TRICK_ICG_INCLUDE_DIR)
    target_sources(${target} PRIVATE ${sources})
    target_include_directories(${target} PRIVATE "${include_dir}")
    add_dependencies(${target} trick_generation)
endfunction()

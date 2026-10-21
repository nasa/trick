# Evaluate usage requirements at generation time. HDF5 intentionally remains
# excluded from ICG's historical parsing environment.
include_guard(GLOBAL)
include("${CMAKE_CURRENT_LIST_DIR}/TrickICGCommand.cmake")

function(_trick_icg_probe_compiler out_compiler out_has_sysroot)
    # Probe the same compiler and sysroot used to build Trick, not the caller's SDK.
    set(compiler_args "${CMAKE_CXX_COMPILER}")
    if(CMAKE_CXX_COMPILER_ARG1)
        separate_arguments(compiler_arg1 NATIVE_COMMAND "${CMAKE_CXX_COMPILER_ARG1}")
        list(APPEND compiler_args ${compiler_arg1})
    endif()
    set(sysroot "${CMAKE_SYSROOT}")
    if(DEFINED CMAKE_SYSROOT_COMPILE)
        set(sysroot "${CMAKE_SYSROOT_COMPILE}")
    endif()
    set(has_sysroot FALSE)
    if(sysroot)
        list(APPEND compiler_args "--sysroot=${sysroot}")
        set(has_sysroot TRUE)
    endif()
    if(APPLE AND CMAKE_OSX_SYSROOT)
        set(sdk "${CMAKE_OSX_SYSROOT}")
        if(NOT IS_ABSOLUTE "${sdk}")
            # Xcode can retain an SDK name instead of its path.
            execute_process(COMMAND xcrun --sdk "${sdk}" --show-sdk-path
                OUTPUT_VARIABLE sdk OUTPUT_STRIP_TRAILING_WHITESPACE
                COMMAND_ERROR_IS_FATAL ANY)
        endif()
        list(APPEND compiler_args -isysroot "${sdk}")
        set(has_sysroot TRUE)
    endif()
    # ICG runs TRICK_CXX through popen; quote each command argument for the shell.
    set(compiler "")
    foreach(part IN LISTS compiler_args)
        string(REPLACE "'" "'\\''" part "${part}")
        string(APPEND compiler "'${part}' ")
    endforeach()
    string(STRIP "${compiler}" compiler)
    set(${out_compiler} "${compiler}" PARENT_SCOPE)
    set(${out_has_sysroot} "${has_sysroot}" PARENT_SCOPE)
endfunction()

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

    _trick_icg_probe_compiler(compiler has_sysroot)
    file(GENERATE OUTPUT "${settings}" CONTENT
"set(icg_local_includes [==[$<TARGET_PROPERTY:trick_headers,INTERFACE_INCLUDE_DIRECTORIES>]==])
set(icg_external_includes [==[$<TARGET_PROPERTY:trick_icg_external,INTERFACE_INCLUDE_DIRECTORIES>]==])
set(icg_definitions [==[$<TARGET_PROPERTY:trick_icg_usage,INTERFACE_COMPILE_DEFINITIONS>]==])
set(icg_implicit_includes [==[${CMAKE_CXX_IMPLICIT_INCLUDE_DIRECTORIES}]==])
set(icg_source_dir [==[${PROJECT_SOURCE_DIR}]==])
set(icg_binary_dir [==[${PROJECT_BINARY_DIR}]==])
set(icg_compiler [==[${compiler}]==])
set(icg_has_sysroot ${has_sysroot})
set(icg_outputs [==[${outputs}]==])
set(icg_verbose [==[${CMAKE_VERBOSE_MAKEFILE}]==])
" TARGET trick_icg_usage)

    _trick_icg_command("${root}" "${settings}" trick-ICG
        "${PROJECT_SOURCE_DIR}/include/trick/files_to_ICG.hh" ${byproducts})
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

        find_program(test_ninja NAMES ninja NO_CACHE)
        find_program(test_make NAMES gmake make NO_CACHE)
        add_test(NAME icg_depfile_cli
            COMMAND "${CMAKE_COMMAND}" "-DICG=$<TARGET_FILE:trick-ICG>"
                "-DICG_SETTINGS=${settings}"
                "-DICG_TEST_ROOT=${PROJECT_BINARY_DIR}/tests/$<CONFIG>/icg-depfile-cli"
                -DTEST_MODE=cli
                -P "${scripts}/TestTrickICGDepfileCLI.cmake")
        set_tests_properties(icg_depfile_cli PROPERTIES LABELS "codegen" TIMEOUT 300)
        add_test(NAME icg_depfile_quoting
            COMMAND "${CMAKE_COMMAND}" "-DICG=$<TARGET_FILE:trick-ICG>"
                "-DICG_SETTINGS=${settings}"
                "-DICG_TEST_ROOT=${PROJECT_BINARY_DIR}/tests/$<CONFIG>/icg-depfile-quoting"
                -DTEST_MODE=quoting "-DTEST_MAKE_PROGRAM=${test_make}"
                -P "${scripts}/TestTrickICGDepfileCLI.cmake")
        set_tests_properties(icg_depfile_quoting PROPERTIES LABELS "codegen" TIMEOUT 300)
        if(NOT test_make)
            set_tests_properties(icg_depfile_quoting PROPERTIES DISABLED TRUE)
        endif()

        set(drivers ninja make)
        set(generators "Ninja" "Unix Makefiles")
        foreach(driver generator IN ZIP_LISTS drivers generators)
            add_test(NAME icg_depfile_${driver}
                COMMAND "${CMAKE_COMMAND}" "-DICG=$<TARGET_FILE:trick-ICG>"
                    "-DICG_SETTINGS=${settings}"
                    "-DICG_TEST_ROOT=${PROJECT_BINARY_DIR}/tests/$<CONFIG>/icg-depfile-${driver}"
                    "-DTEST_GENERATOR=${generator}" "-DTEST_MAKE_PROGRAM=${test_${driver}}"
                    -P "${scripts}/TestTrickICGDepfile.cmake")
            set_tests_properties(icg_depfile_${driver}
                PROPERTIES LABELS "codegen;slow" TIMEOUT 300)
            if(NOT test_${driver})
                set_tests_properties(icg_depfile_${driver} PROPERTIES DISABLED TRUE)
            endif()
        endforeach()
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

cmake_minimum_required(VERSION 3.26)
# Exercise ICG directly; no nested CMake project or build driver is needed.
if(NOT TEST_MODE MATCHES "^(cli|quoting)$")
    message(FATAL_ERROR "TEST_MODE must be cli or quoting")
endif()
if(TEST_MODE STREQUAL "quoting" AND NOT TEST_MAKE_PROGRAM)
    message(FATAL_ERROR "Target-quoting checks require Make")
endif()
set(scripts "${CMAKE_CURRENT_LIST_DIR}")
set(system_name "system #$.hh")
include("${scripts}/tests/icg-depfile/CreateFixture.cmake")

set(ICG_SETTINGS "${work}/settings.cmake")
include("${scripts}/TrickICGInvoke.cmake")
if(TEST_MODE STREQUAL "cli")
    foreach(option IN ITEMS --depfile --depfile-target)
        execute_process(COMMAND "${ICG}" "${option}" "${work}/unused"
            TIMEOUT 120
            RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE output)
        if(result EQUAL 0 OR NOT output MATCHES "must be supplied together")
            message(FATAL_ERROR "ICG accepted an incomplete depfile option pair:\n${output}")
        endif()
    endforeach()
    trick_icg_invoke("${work}/probe" "${home}" result
        DEPFILE "${work}/missing-directory/probe.d" DEPFILE_TARGET "probe")
    if(result EQUAL 0)
        message(FATAL_ERROR "ICG did not report a depfile write failure")
    endif()

else()
    # GNU Make reads the raw depfile, without CMake's version-specific conversion.
    foreach(target IN ITEMS "target with spaces" "target#dollar$" "target:colon"
            "literal\\backslash" "before\\#hash" "trailing\\")
        trick_icg_invoke("${work}/probe" "${home}" result
            DEPFILE "${work}/probe.d" DEPFILE_TARGET "${target}")
        if(NOT result EQUAL 0)
            message(FATAL_ERROR "ICG failed to emit a quoted depfile target")
        endif()
        execute_process(COMMAND "${TEST_MAKE_PROGRAM}" -f "${work}/probe.d" "${target}"
            WORKING_DIRECTORY "${work}"
            RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE output)
        if(NOT result EQUAL 0)
            message(FATAL_ERROR "Make could not read target '${target}':\n${output}")
        endif()
    endforeach()
endif()

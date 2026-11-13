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

    foreach(failure IN ITEMS missing-include invalid-class)
        if(failure STREQUAL "missing-include")
            file(WRITE "${local}"
                "#include \"MissingICGType.hh\"\nclass Local { public: MissingICGType value; MissingICGType values[2]; };\n")
            set(expected_error "MissingICGType.hh.*file not found")
        else()
            file(WRITE "${local}"
                "class Local { public: Local value; };\n")
            set(expected_error "incomplete type")
        endif()
        set(depfile "${work}/${failure}.d")
        set(sentinel "Existing depfile must survive a parse failure.\n")
        file(WRITE "${depfile}" "${sentinel}")
        trick_icg_invoke("${work}/${failure}" "${home}" result
            DEPFILE "${depfile}" DEPFILE_TARGET "probe"
            TIMEOUT 120 OUTPUT_VARIABLE output)
        if(NOT result MATCHES "^[0-9]+$" OR result EQUAL 0
                OR NOT output MATCHES "${expected_error}"
                OR NOT output MATCHES "Trick build was terminated due to error in user code")
            message(FATAL_ERROR
                "ICG did not reject ${failure} with a normal error exit (${result}):\n${output}")
        endif()
        file(READ "${depfile}" actual)
        if(NOT actual STREQUAL sentinel)
            message(FATAL_ERROR "ICG replaced the depfile after ${failure}")
        endif()
    endforeach()

    # A suppressed system error must not send an invalid record to Clang's
    # layout engine, even through a valid typedef in a user header.
    file(WRITE "${system}"
        "struct Incomplete;\nstruct InvalidSystemRecord { Incomplete member; };\n")
    file(WRITE "${local}"
        "#include <${system_name}>\ntypedef InvalidSystemRecord InvalidAlias;\nstruct Opaque;\ntypedef Opaque OpaqueAlias;\nstruct ValidAfterInvalid { int value; };\n")
    file(WRITE "${main}" "#include \"Local.hh\"\n")
    trick_icg_invoke("${work}/invalid-system-record" "${home}" result
        DEPFILE "${work}/invalid-system-record.d" DEPFILE_TARGET "probe"
        TIMEOUT 120 OUTPUT_VARIABLE output)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "ICG failed on a suppressed system error (${result}):\n${output}")
    endif()
    string(FIND "${output}" "${system}:2: skipping invalid record 'InvalidSystemRecord'" warning)
    if(warning EQUAL -1)
        message(FATAL_ERROR "ICG did not identify the skipped record and its location:\n${output}")
    endif()
    if(output MATCHES "skipping invalid record 'Opaque")
        message(FATAL_ERROR "ICG warned about an ordinary forward declaration:\n${output}")
    endif()
    file(READ "${work}/invalid-system-record/sim_services/io_src/io_Local.cpp" generated)
    if(NOT generated MATCHES "ValidAfterInvalid" OR generated MATCHES "InvalidAlias")
        message(FATAL_ERROR "ICG skipped valid metadata or emitted an invalid system record")
    endif()
    if(NOT EXISTS "${work}/invalid-system-record.d")
        message(FATAL_ERROR "ICG did not emit dependencies after a suppressed system error")
    endif()

    # Clang may mark a user class invalid without diagnosing it when its field
    # uses an invalid system type. ICG must fail instead of silently dropping it.
    file(WRITE "${local}"
        "#include <${system_name}>\nclass User { public: InvalidSystemRecord value; };\n")
    set(depfile "${work}/invalid-user-record.d")
    file(WRITE "${depfile}" "${sentinel}")
    trick_icg_invoke("${work}/invalid-user-record" "${home}" result
        DEPFILE "${depfile}" DEPFILE_TARGET "probe"
        TIMEOUT 120 OUTPUT_VARIABLE output)
    if(NOT result MATCHES "^[0-9]+$" OR result EQUAL 0
            OR NOT output MATCHES "cannot generate attributes for invalid user record 'User'"
            OR NOT output MATCHES "Trick build was terminated due to error in user code")
        message(FATAL_ERROR "ICG did not reject an invalid user record (${result}):\n${output}")
    endif()
    string(FIND "${output}" "${local}:2:" location)
    if(location EQUAL -1)
        message(FATAL_ERROR "ICG did not locate the invalid user record:\n${output}")
    endif()
    file(READ "${depfile}" actual)
    if(NOT actual STREQUAL sentinel)
        message(FATAL_ERROR "ICG replaced the depfile after an invalid user record")
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

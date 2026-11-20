# Shared by production and direct-invocation regression tests.
include("${ICG_SETTINGS}")
set(flags --icg-std=c++17)
list(REMOVE_DUPLICATES icg_local_includes)
list(REMOVE_DUPLICATES icg_external_includes)
list(REMOVE_DUPLICATES icg_definitions)
foreach(path IN LISTS icg_local_includes)
    list(APPEND flags "-I${path}")
endforeach()
# The compiler searches these already; -isystem would reorder them ahead of
# the C++ standard library headers and break its #include_next chain.
foreach(path IN LISTS icg_external_includes)
    if(NOT path IN_LIST icg_local_includes AND NOT path IN_LIST icg_implicit_includes)
        list(APPEND flags "-isystem${path}")
    endif()
endforeach()
foreach(definition IN LISTS icg_definitions)
    if(NOT definition STREQUAL "")
        list(APPEND flags "-D${definition}")
    endif()
endforeach()

function(trick_icg_filter_progress output result_var)
    # Do not split diagnostics into CMake lists: semicolons and ANSI '['
    # sequences have list syntax. Consume whole lines as strings instead.
    string(ASCII 27 escape)
    set(filtered "")
    while(NOT output STREQUAL "")
        string(FIND "${output}" "\n" newline)
        if(newline EQUAL -1)
            set(line "${output}")
            set(output "")
        else()
            math(EXPR length "${newline} + 1")
            string(SUBSTRING "${output}" 0 ${length} line)
            string(SUBSTRING "${output}" ${length} -1 output)
        endif()
        if(NOT line MATCHES "^(${escape}\\[[0-9;]*m)*Writing[ \t]+")
            string(APPEND filtered "${line}")
        endif()
    endwhile()
    set(${result_var} "${filtered}" PARENT_SCOPE)
endfunction()

function(trick_icg_invoke output_root trick_home result_var)
    cmake_parse_arguments(PARSE_ARGV 3 arg "" "DEPFILE;DEPFILE_TARGET;TIMEOUT;OUTPUT_VARIABLE" "")
    set(dependency_flags)
    if(DEFINED arg_DEPFILE OR DEFINED arg_DEPFILE_TARGET)
        list(APPEND dependency_flags
            --depfile "${arg_DEPFILE}" --depfile-target "${arg_DEPFILE_TARGET}")
    endif()
    set(timeout)
    if(DEFINED arg_TIMEOUT)
        set(timeout TIMEOUT "${arg_TIMEOUT}")
    endif()
    # Run ICG directly so a timeout terminates ICG, not just a cmake -E env
    # wrapper that could leave it running. This also retains signal results.
    # Restore the caller's environment; function scope does not restore ENV values.
    set(environment TRICK_HOME TRICK_CXX
        TRICK_ICG_EXCLUDE TRICK_ICG_IGNORE_TYPES TRICK_SYSTEM_ICG_EXCLUDE
        TRICK_ICG_COMPAT15 TRICK_ICG_NOCOMMENT TRICK_EXCLUDE
        TRICK_EXT_LIB_DIRS TRICK_EXT_LIB_DIRS_OVERRIDES)
    foreach(variable IN LISTS environment)
        if(DEFINED ENV{${variable}})
            set(saved_${variable} "$ENV{${variable}}")
        endif()
        unset(ENV{${variable}})
    endforeach()
    set(ENV{TRICK_HOME} "${trick_home}")
    set(ENV{TRICK_CXX} "${icg_compiler}")
    execute_process(
        COMMAND "${ICG}" -sim_services -m --output-root "${output_root}" ${flags} ${dependency_flags}
            "${icg_source_dir}/include/trick/files_to_ICG.hh"
        WORKING_DIRECTORY "${icg_binary_dir}"
        ${timeout}
        RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE output)
    foreach(variable IN LISTS environment)
        if(DEFINED saved_${variable})
            set(ENV{${variable}} "${saved_${variable}}")
        else()
            unset(ENV{${variable}})
        endif()
    endforeach()
    if(DEFINED arg_OUTPUT_VARIABLE)
        set(${arg_OUTPUT_VARIABLE} "${output}" PARENT_SCOPE)
    endif()
    # Keep warnings and diagnostics visible. Only routine per-file progress is
    # hidden on success; failures and verbose runs retain the complete output.
    # Ninja --verbose only prints commands; use VERBOSE=1 or
    # -DCMAKE_VERBOSE_MAKEFILE=ON to show ICG progress with either generator.
    if(result STREQUAL "0" AND NOT icg_verbose AND NOT DEFINED ENV{VERBOSE})
        trick_icg_filter_progress("${output}" output)
    endif()
    if(NOT output STREQUAL "")
        message("${output}")
    endif()
    set(${result_var} "${result}" PARENT_SCOPE)
endfunction()

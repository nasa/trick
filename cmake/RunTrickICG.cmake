cmake_minimum_required(VERSION 3.25)
include("${CMAKE_CURRENT_LIST_DIR}/TrickICGInvoke.cmake")
if(NOT IS_ABSOLUTE "${ICG_OUTPUT_ROOT}")
    message(FATAL_ERROR "ICG_OUTPUT_ROOT must be an absolute build-tree path")
endif()
cmake_path(NORMAL_PATH ICG_OUTPUT_ROOT OUTPUT_VARIABLE root)
set(expected ${icg_outputs})
set(stamp "${root}.complete")
set(stage "${root}.stage")
# file(REMOVE[_RECURSE]) can silently fail; cmake -E rm reports removal errors.
execute_process(COMMAND "${CMAKE_COMMAND}" -E rm -rf "${stage}"
    COMMAND_ERROR_IS_FATAL ANY)
file(MAKE_DIRECTORY "${stage}")
trick_icg_invoke("${stage}" "${icg_source_dir}" result)
if(NOT result EQUAL 0)
    message(FATAL_ERROR "Trick ICG failed: ${result}; published outputs unchanged")
endif()
file(GLOB_RECURSE actual LIST_DIRECTORIES FALSE RELATIVE "${stage}" "${stage}/*")
list(SORT actual)
list(SORT expected)
if(NOT actual STREQUAL expected)
    set(missing ${expected})
    set(unexpected ${actual})
    list(REMOVE_ITEM missing ${actual})
    list(REMOVE_ITEM unexpected ${expected})
    foreach(kind IN ITEMS missing unexpected)
        if(NOT ${kind})
            set(${kind} "(none)")
        endif()
        list(JOIN ${kind} "\n  " ${kind})
    endforeach()
    message(FATAL_ERROR
        "ICG output inventory mismatch.\n"
        "Update cmake/TrickICGOutputs.cmake:\n"
        "  _icg_core_names for core headers\n"
        "  _icg_er7_headers for er7_utils headers\n"
        "Missing:\n  ${missing}\nUnexpected:\n  ${unexpected}")
endif()
# Publish only after successful generation AND complete inventory validation.
# Use error-reporting removals so failed publication cannot leave a valid stamp.
execute_process(COMMAND "${CMAKE_COMMAND}" -E rm -f "${stamp}"
    COMMAND_ERROR_IS_FATAL ANY)
foreach(relative IN LISTS expected)
    get_filename_component(directory "${root}/${relative}" DIRECTORY)
    file(MAKE_DIRECTORY "${directory}")
    file(COPY_FILE "${stage}/${relative}" "${root}/${relative}" ONLY_IF_DIFFERENT)
endforeach()
file(GLOB_RECURSE previous LIST_DIRECTORIES FALSE RELATIVE "${root}" "${root}/*")
foreach(path IN LISTS previous)
    if(NOT path IN_LIST expected)
        execute_process(COMMAND "${CMAKE_COMMAND}" -E rm -f "${root}/${path}"
            COMMAND_ERROR_IS_FATAL ANY)
    endif()
endforeach()
execute_process(COMMAND "${CMAKE_COMMAND}" -E rm -rf "${stage}"
    COMMAND_ERROR_IS_FATAL ANY)
file(TOUCH "${stamp}")

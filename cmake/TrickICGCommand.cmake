# The production generation rule, also used by the build-driver fixture.
include_guard(GLOBAL)

# _trick_icg_command(root settings icg_target entry_header <byproducts...>)
function(_trick_icg_command root settings icg_target entry_header)
    set(scripts "${CMAKE_CURRENT_FUNCTION_LIST_DIR}")
    # Keep the entry header explicit even if learned dependency records are lost.
    add_custom_command(
        OUTPUT "${root}.complete"
        BYPRODUCTS ${ARGN} "${root}.d"
        COMMAND "${CMAKE_COMMAND}" "-DICG=$<TARGET_FILE:${icg_target}>"
            "-DICG_SETTINGS=${settings}" "-DICG_OUTPUT_ROOT=${root}"
            -P "${scripts}/RunTrickICG.cmake"
        DEPENDS ${icg_target} "${entry_header}" "${settings}"
            "${CMAKE_CURRENT_FUNCTION_LIST_FILE}"
            "${scripts}/TrickICGInvoke.cmake" "${scripts}/RunTrickICG.cmake"
        DEPFILE "${root}.d"
        COMMENT "Generating Trick core IO, maps and XML"
        VERBATIM)
endfunction()

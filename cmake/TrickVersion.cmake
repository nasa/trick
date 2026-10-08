# Reads Trick's version from share/trick/trick_ver.txt, the file trick-version
# and release tooling already treat as authoritative. Included before project(),
# which takes the numeric part.
#
# Sets:
#   TRICK_VERSION             numeric version for project(), e.g. 25.1.2
#   TRICK_VERSION_PRERELEASE  suffix without the dash, e.g. beta (may be empty)
#   TRICK_VERSION_FULL        what trick-version prints, e.g. 25.1.2-beta

set(_trick_ver_file "${CMAKE_CURRENT_LIST_DIR}/../share/trick/trick_ver.txt")
cmake_path(NORMAL_PATH _trick_ver_file)

file(STRINGS "${_trick_ver_file}" _trick_ver_line REGEX "^current_version")
if(NOT _trick_ver_line MATCHES [["(trick-)?([0-9]+)\.([0-9]+)\.([0-9]+)(-([0-9A-Za-z.]+))?"]])
    message(FATAL_ERROR "Could not parse current_version from ${_trick_ver_file}")
endif()

set(TRICK_VERSION "${CMAKE_MATCH_2}.${CMAKE_MATCH_3}.${CMAKE_MATCH_4}")
set(TRICK_VERSION_PRERELEASE "${CMAKE_MATCH_6}")
set(TRICK_VERSION_FULL "${TRICK_VERSION}${CMAKE_MATCH_5}")

# Re-run configure when the version changes.
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${_trick_ver_file}")

unset(_trick_ver_file)
unset(_trick_ver_line)

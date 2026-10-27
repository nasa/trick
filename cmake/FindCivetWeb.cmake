# FindCivetWeb
# ------------
#
# Finds a CivetWeb installation that has no CMake package, such as one built
# with CivetWeb's own makefile (`make lib WITH_CPP=1`). Prefer CivetWeb's CMake
# package when it is available; TrickDependencies.cmake tries that first.
#
# Trick uses CivetWeb's C++ API, so CivetServer.h is required and the library
# must have been built with C++ support. CivetWeb links against zlib.
#
# Set CivetWeb_ROOT to search a specific installation first.
#
# Imported target, named to match CivetWeb's own CMake package:
#   civetweb::civetweb-cpp
#
# Result variables:
#   CivetWeb_FOUND

find_path(CivetWeb_INCLUDE_DIR
    NAMES CivetServer.h
    DOC "Directory containing civetweb.h and CivetServer.h")
find_library(CivetWeb_LIBRARY
    NAMES civetweb
    DOC "CivetWeb library, built with C++ support")
mark_as_advanced(CivetWeb_INCLUDE_DIR CivetWeb_LIBRARY)

find_package(ZLIB QUIET)
find_package(Threads QUIET)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(CivetWeb
    REQUIRED_VARS CivetWeb_LIBRARY CivetWeb_INCLUDE_DIR ZLIB_FOUND Threads_FOUND)

if(CivetWeb_FOUND AND NOT TARGET civetweb::civetweb-cpp)
    add_library(civetweb::civetweb-cpp UNKNOWN IMPORTED)
    set_target_properties(civetweb::civetweb-cpp PROPERTIES
        IMPORTED_LOCATION "${CivetWeb_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${CivetWeb_INCLUDE_DIR}"
        INTERFACE_LINK_LIBRARIES "ZLIB::ZLIB;Threads::Threads;${CMAKE_DL_LIBS}")
endif()

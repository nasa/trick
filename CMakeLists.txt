cmake_minimum_required(VERSION 3.25...4.4)

# Building in the source tree would overwrite Trick's autotools Makefile and
# mix generated files into the checkout.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_BINARY_DIR)
    message(FATAL_ERROR
        "In-source builds are not supported. Use a build directory, for example:\n"
        "  cmake --preset dev\n"
        "Remove the CMakeCache.txt and CMakeFiles/ this attempt created.")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(TrickVersion)

project(Trick
    VERSION ${TRICK_VERSION}
    DESCRIPTION "NASA's Trick Simulation Environment"
    HOMEPAGE_URL "https://github.com/nasa/trick"
    LANGUAGES C CXX)

include(GNUInstallDirs)
include(TrickOptions)
include(TrickDependencies)
include(TrickLibraries)

add_subdirectory(trick_source)

if(PROJECT_IS_TOP_LEVEL)
    feature_summary(WHAT ENABLED_FEATURES DISABLED_FEATURES PACKAGES_FOUND INCLUDE_QUIET_PACKAGES
        DESCRIPTION "Trick ${TRICK_VERSION_FULL}")
endif()

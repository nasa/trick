# User-facing build options.
#
# Every optional component is explicit: an option that is ON requires its
# dependencies, and configure fails if they are missing. Nothing is enabled
# because it happens to be installed. Defaults match what a typical Trick
# build produces today.

include(CMakeDependentOption)

option(TRICK_ENABLE_ER7_UTILS
    "Build er7_utils and use its integrators in place of Trick's built-in ones" ON)

option(TRICK_ENABLE_JAVA
    "Build the Java applications (trick-tv, trick-dp, trick-simcontrol, ...)" ON)
set(TRICK_JAVA_OFFLINE_DIR "" CACHE PATH
    "Directory of prebuilt Trick jars to install instead of building them with Maven")

option(TRICK_ENABLE_DATA_PRODUCTS
    "Build the data products libraries and applications (trick-trk2csv, DPX, ...)" ON)
cmake_dependent_option(TRICK_ENABLE_X11_APPS
    "Build the X11 data products application trick-gxplot" ON
    "TRICK_ENABLE_DATA_PRODUCTS" OFF)

option(TRICK_ENABLE_HDF5 "Support HDF5 data recording" OFF)
option(TRICK_ENABLE_GSL "Use GSL random number generators in Trick's math utilities and Monte Carlo" OFF)
option(TRICK_ENABLE_CIVETWEB "Build the CivetWeb web server library (libtrickCivet)" OFF)
option(TRICK_ENABLE_DOCS "Build the Doxygen reference documentation" OFF)

# Tests are for people working on Trick. They are off by default so that
# building Trick to use it does not require GoogleTest, and never built when
# Trick is part of another project.
option(BUILD_TESTING "Build Trick's unit and simulation tests" OFF)
if(PROJECT_IS_TOP_LEVEL AND BUILD_TESTING)
    set(TRICK_BUILD_TESTS ON)
    enable_testing()
else()
    set(TRICK_BUILD_TESTS OFF)
endif()

include(FeatureSummary)
# Feature names must differ from package names; FeatureSummary stores both
# under the same key.
add_feature_info("er7_utils integrators" TRICK_ENABLE_ER7_UTILS "")
add_feature_info("Java applications" TRICK_ENABLE_JAVA "")
add_feature_info("Data products" TRICK_ENABLE_DATA_PRODUCTS "")
add_feature_info("trick-gxplot" TRICK_ENABLE_X11_APPS "")
add_feature_info("HDF5 data recording" TRICK_ENABLE_HDF5 "")
add_feature_info("GSL random numbers" TRICK_ENABLE_GSL "")
add_feature_info("Web server (libtrickCivet)" TRICK_ENABLE_CIVETWEB "")
add_feature_info("Reference documentation" TRICK_ENABLE_DOCS "")
add_feature_info("Tests" TRICK_BUILD_TESTS "")

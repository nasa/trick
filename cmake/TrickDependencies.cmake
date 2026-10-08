# Finds everything Trick's build needs, in one place.
#
# Dependencies are brought in as imported targets. Anything required fails
# configure immediately. Optional components are found only when their
# TRICK_ENABLE_* option is ON, and are then required.
#
# To point at a specific installation, use CMake's standard hints:
#   <Package>_ROOT, <Package>_DIR, or CMAKE_PREFIX_PATH
# for example -DClang_DIR=/usr/lib/llvm-18/lib/cmake/clang or
# -DPython_EXECUTABLE=/usr/bin/python2.

include(FeatureSummary)

#
# Compiler
#
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS 8.5)
    message(FATAL_ERROR "Trick requires GCC 8.5 or newer; found ${CMAKE_CXX_COMPILER_VERSION}")
endif()

#
# Always required
#
find_package(Threads REQUIRED)

find_package(UDUNITS2 REQUIRED)
set_package_properties(UDUNITS2 PROPERTIES
    URL "https://www.unidata.ucar.edu/software/udunits/"
    PURPOSE "Units conversion in the Trick runtime and ICG")

# Trick embeds Python in every simulation. Python 2.7 is still supported;
# select an interpreter with -DPython_EXECUTABLE=... .
#
# Like ./configure, prefer the python3 (or python) on PATH over the newest
# versioned interpreter. Some distributions install a newer python3.X for their
# own tools without its development files, e.g. LLVM on Enterprise Linux 8.
if(NOT DEFINED Python_FIND_UNVERSIONED_NAMES)
    set(Python_FIND_UNVERSIONED_NAMES FIRST)
endif()
find_package(Python REQUIRED COMPONENTS Interpreter Development.Embed)
set_package_properties(Python PROPERTIES
    DESCRIPTION "Python ${Python_VERSION} (${Python_EXECUTABLE})"
    PURPOSE "Embedded input processor for simulations")

find_package(SWIG 3.0 REQUIRED COMPONENTS python)
set_package_properties(SWIG PROPERTIES
    PURPOSE "Generates Trick's Python interface")

find_package(FLEX REQUIRED)
# macOS ships Bison 2.3. Prefer Homebrew's keg-only Bison, as ./configure
# does, but preserve an explicit BISON_EXECUTABLE (including a cached choice).
if(APPLE AND NOT BISON_EXECUTABLE)
    execute_process(COMMAND brew --prefix bison
        RESULT_VARIABLE _trick_bison_prefix_result
        OUTPUT_VARIABLE _trick_bison_prefix
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET)
    if(_trick_bison_prefix_result STREQUAL "0" AND _trick_bison_prefix)
        find_program(BISON_EXECUTABLE NAMES bison
            HINTS "${_trick_bison_prefix}/bin" NO_DEFAULT_PATH)
    endif()
    unset(_trick_bison_prefix_result)
    unset(_trick_bison_prefix)
endif()
find_package(BISON 3.0 REQUIRED)
set_package_properties(FLEX PROPERTIES PURPOSE "Memory manager and checkpoint parsers")
set_package_properties(BISON PROPERTIES PURPOSE "Memory manager and checkpoint parsers")

# ICG links against Clang's libraries. Clang's package brings in LLVM's.
# Homebrew's LLVM is keg-only; its prefixes are searched after the default
# locations, matching what ./configure does.
find_package(Clang REQUIRED CONFIG
    PATHS /opt/homebrew/opt/llvm /usr/local/opt/llvm)
if(LLVM_VERSION_MAJOR VERSION_LESS 14)
    message(FATAL_ERROR "Trick requires LLVM/Clang 14 or newer; found ${LLVM_PACKAGE_VERSION} in ${LLVM_DIR}")
endif()
set_package_properties(Clang PROPERTIES
    DESCRIPTION "Clang ${LLVM_PACKAGE_VERSION} libraries (${Clang_DIR})"
    PURPOSE "Parses headers for the Interface Code Generator (trick-ICG)")

# Trick's scripts (trick-CP, ICG helpers, trick-config) are Perl.
find_package(Perl 5.14 REQUIRED)
foreach(_trick_perl_module IN ITEMS Text::Balanced Digest::MD5)
    execute_process(
        COMMAND "${PERL_EXECUTABLE}" "-M${_trick_perl_module}" -e 1
        RESULT_VARIABLE _trick_perl_module_missing
        OUTPUT_QUIET ERROR_QUIET)
    if(_trick_perl_module_missing)
        message(FATAL_ERROR "Trick requires the Perl module ${_trick_perl_module}")
    endif()
endforeach()
unset(_trick_perl_module)
unset(_trick_perl_module_missing)
set_package_properties(Perl PROPERTIES PURPOSE "Trick's build and simulation scripts")

# Simulation builds run these; config_user.mk records tee's location.
find_program(TEE_EXECUTABLE tee REQUIRED)
find_program(ZIP_EXECUTABLE zip REQUIRED)
mark_as_advanced(TEE_EXECUTABLE ZIP_EXECUTABLE)

#
# Optional components
#
if(TRICK_ENABLE_JAVA)
    if(TRICK_JAVA_OFFLINE_DIR)
        # Offline builds install prebuilt jars, so they only need a Java
        # runtime, not a JDK or Maven.
        if(NOT IS_DIRECTORY "${TRICK_JAVA_OFFLINE_DIR}")
            message(FATAL_ERROR "TRICK_JAVA_OFFLINE_DIR is not a directory: ${TRICK_JAVA_OFFLINE_DIR}")
        endif()
        find_package(Java 11 REQUIRED COMPONENTS Runtime)
    else()
        find_package(Java 11 REQUIRED COMPONENTS Development)
        find_program(MAVEN_EXECUTABLE mvn REQUIRED)
        mark_as_advanced(MAVEN_EXECUTABLE)
    endif()
    set_package_properties(Java PROPERTIES PURPOSE "Java applications")
endif()

if(TRICK_ENABLE_DATA_PRODUCTS)
    find_package(LibXml2 REQUIRED)
    set_package_properties(LibXml2 PROPERTIES PURPOSE "Data products (DPX)")
endif()

if(TRICK_ENABLE_X11_APPS)
    find_package(X11 REQUIRED)
    if(NOT X11_Xt_FOUND)
        message(FATAL_ERROR "TRICK_ENABLE_X11_APPS requires the X Toolkit (libXt) development files")
    endif()
    set_package_properties(X11 PROPERTIES PURPOSE "trick-gxplot")
endif()

if(TRICK_ENABLE_HDF5)
    find_package(HDF5 REQUIRED COMPONENTS C HL)
    set_package_properties(HDF5 PROPERTIES PURPOSE "HDF5 data recording")
endif()

if(TRICK_ENABLE_GSL)
    find_package(GSL REQUIRED)
    set_package_properties(GSL PROPERTIES PURPOSE "GSL random number generators")
endif()

if(TRICK_ENABLE_CIVETWEB)
    # Prefer CivetWeb's own package; fall back to searching for a
    # makefile-built installation.
    find_package(civetweb CONFIG QUIET)
    if(TARGET civetweb::civetweb-cpp)
        set_package_properties(civetweb PROPERTIES PURPOSE "Web server (libtrickCivet)")
    else()
        find_package(CivetWeb MODULE REQUIRED)
        set_package_properties(CivetWeb PROPERTIES PURPOSE "Web server (libtrickCivet)")
    endif()
endif()

if(TRICK_ENABLE_DOCS)
    find_package(Doxygen REQUIRED)
    set_package_properties(Doxygen PROPERTIES PURPOSE "Reference documentation")
endif()

if(TRICK_BUILD_TESTS)
    find_package(GTest REQUIRED)
    set_package_properties(GTest PROPERTIES PURPOSE "Unit tests")
endif()

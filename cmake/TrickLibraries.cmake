# Targets and helpers shared by all of Trick's libraries.

# Everything Trick builds is linked into simulation executables, which may be
# position independent.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
# -std=c++17 rather than -std=gnu++17, as the Make build uses.
set(CMAKE_CXX_EXTENSIONS OFF)

# trick_headers: Trick's public headers and the definitions they depend on.
# Simulations receive the same include paths and definitions through
# TRICK_SYSTEM_CXXFLAGS; changing one without the other changes what the
# headers mean.
add_library(trick_headers INTERFACE)
add_library(Trick::headers ALIAS trick_headers)
set_target_properties(trick_headers PROPERTIES EXPORT_NAME headers)
target_include_directories(trick_headers INTERFACE
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>"
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/trick/compat>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}/trick/compat>")
target_compile_features(trick_headers INTERFACE cxx_std_17)
target_compile_definitions(trick_headers INTERFACE
    TRICK_VER=${PROJECT_VERSION_MAJOR}
    TRICK_MINOR=${PROJECT_VERSION_MINOR}
    $<$<BOOL:${TRICK_ENABLE_ER7_UTILS}>:USE_ER7_UTILS_INTEGRATORS>
    $<$<BOOL:${TRICK_ENABLE_GSL}>:_HAVE_GSL>
    $<$<BOOL:${TRICK_ENABLE_CIVETWEB}>:USE_CIVETWEB>)

# trick_build_options: settings for compiling Trick itself that are not passed
# on to anything that links against Trick. Link it with
# $<BUILD_INTERFACE:trick_build_options> so it never appears in an export.
add_library(trick_build_options INTERFACE)
target_compile_options(trick_build_options INTERFACE
    # Let C++ exceptions propagate through Trick's C code.
    $<$<COMPILE_LANGUAGE:C>:-fexceptions>)

# trick_add_library(<name> STATIC|OBJECT [EXPORT_NAME <name>] SOURCES <file>...)
#
# Adds a Trick library that uses Trick's headers. A STATIC library's archive is
# named lib<name>.a, which simulations link by name. EXPORT_NAME also creates
# the alias Trick::<export name>, the name other projects will use.
function(trick_add_library name type)
    cmake_parse_arguments(PARSE_ARGV 2 arg "" "EXPORT_NAME" "SOURCES")
    if(arg_UNPARSED_ARGUMENTS OR NOT arg_SOURCES)
        message(FATAL_ERROR "trick_add_library(${name}): expected SOURCES, got: ${ARGN}")
    endif()
    add_library(${name} ${type} ${arg_SOURCES})
    target_link_libraries(${name}
        PUBLIC trick_headers
        PRIVATE $<BUILD_INTERFACE:trick_build_options>)
    if(arg_EXPORT_NAME)
        set_target_properties(${name} PROPERTIES EXPORT_NAME ${arg_EXPORT_NAME})
        add_library(Trick::${arg_EXPORT_NAME} ALIAS ${name})
    endif()
endfunction()

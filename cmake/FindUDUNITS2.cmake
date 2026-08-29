# UDUNITS2 detection, mirroring autoconf/m4/tr_udunits_home.m4 and
# configure.ac:260-302 exactly, including the exact rendered
# UDUNITS_INCLUDES / UDUNITS_LDFLAGS / UDUNITS_EXCLUDE strings
# config_user.mk consumes (TRICK_EXCLUDE += :@UDUNITS_EXCLUDE@ requires the
# variable to render empty rather than unset).
#
# Honors the UDUNITS_HOME cache variable (equivalent to --with-udunits=DIR).
#
# Exports: UDUNITS_INCLUDES, UDUNITS_LDFLAGS, UDUNITS_EXCLUDE
# Also defines an imported target Trick::udunits2 for later phases.

set(UDUNITS_EXCLUDE "")

if(NOT UDUNITS_HOME)
    if(EXISTS "/usr/include/udunits2.h")
        set(UDUNITS_INCLUDES "")
        set(UDUNITS_LDFLAGS "-ludunits2")
        set(_tr_udunits_header "/usr/include/udunits2.h")
    elseif(EXISTS "/usr/include/udunits2/udunits2.h")
        set(UDUNITS_INCLUDES "-I/usr/include/udunits2")
        set(UDUNITS_LDFLAGS "-ludunits2")
        set(_tr_udunits_header "/usr/include/udunits2/udunits2.h")
    elseif(EXISTS "/opt/homebrew/include/udunits2.h")
        set(UDUNITS_HOME "/opt/homebrew")
        set(UDUNITS_INCLUDES "-I${UDUNITS_HOME}/include")
        set(UDUNITS_LDFLAGS "-L${UDUNITS_HOME}/lib -ludunits2")
        set(UDUNITS_EXCLUDE "${UDUNITS_HOME}")
        set(_tr_udunits_header "${UDUNITS_HOME}/include/udunits2.h")
    else()
        message(FATAL_ERROR "could not find udunits2.h")
    endif()
else()
    set(UDUNITS_EXCLUDE "${UDUNITS_HOME}")
    if(EXISTS "${UDUNITS_HOME}/include/udunits2.h")
        set(UDUNITS_INCLUDES "-I${UDUNITS_HOME}/include")
        set(UDUNITS_LDFLAGS "-Wl,-rpath,${UDUNITS_HOME}/lib -L${UDUNITS_HOME}/lib -ludunits2")
        set(_tr_udunits_header "${UDUNITS_HOME}/include/udunits2.h")
    elseif(EXISTS "${UDUNITS_HOME}/lib/udunits2.h")
        set(UDUNITS_INCLUDES "-I${UDUNITS_HOME}/lib")
        set(UDUNITS_LDFLAGS "-Wl,-rpath,${UDUNITS_HOME}/lib -L${UDUNITS_HOME}/lib -ludunits2")
        set(_tr_udunits_header "${UDUNITS_HOME}/lib/udunits2.h")
    else()
        message(FATAL_ERROR "could not find udunits2")
    endif()
endif()

# AC_CHECK_LIB(udunits2, main, ...) — verify the library actually links.
find_library(UDUNITS2_LIBRARY
    NAMES udunits2
    HINTS "${UDUNITS_HOME}/lib"
)
if(NOT UDUNITS2_LIBRARY)
    message(FATAL_ERROR "could not find libudunits")
endif()

if(NOT TARGET Trick::udunits2)
    add_library(Trick::udunits2 UNKNOWN IMPORTED)
    get_filename_component(_tr_udunits_incdir "${_tr_udunits_header}" DIRECTORY)
    set_target_properties(Trick::udunits2 PROPERTIES
        IMPORTED_LOCATION "${UDUNITS2_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${_tr_udunits_incdir}"
    )
endif()

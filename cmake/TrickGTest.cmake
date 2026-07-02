# GTEST_HOME detection + GTEST_CXXSTD derivation, mirroring
# autoconf/m4/tr_gtest.m4.
#
# Honors the GTEST_HOME cache variable; when unset, auto-detects like
# configure's "--with-gtest not given" branch.
#
# Exports: GTEST_HOME, GTEST_CXXSTD

if(GTEST_HOME)
    if(NOT EXISTS "${GTEST_HOME}/include/gtest")
        message(FATAL_ERROR "could not find ${GTEST_HOME}/include/gtest")
    endif()
elseif(EXISTS "/usr/include/gtest/gtest.h")
    set(GTEST_HOME "/usr")
elseif(EXISTS "/opt/homebrew/include/gtest/gtest.h")
    set(GTEST_HOME "/opt/homebrew")
else()
    set(GTEST_HOME "")
endif()

set(GTEST_CXXSTD "")
if(GTEST_HOME)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(TR_GTEST_PC QUIET gtest)
        if(TR_GTEST_PC_VERSION)
            if(TR_GTEST_PC_VERSION VERSION_GREATER_EQUAL "1.17")
                set(GTEST_CXXSTD "-std=c++17")
            else()
                set(GTEST_CXXSTD "-std=c++14")
            endif()
        else()
            message(WARNING "Could not determine gtest version via pkg-config")
        endif()
    else()
        message(WARNING "pkg-config gtest not found, version detection skipped")
    endif()
endif()

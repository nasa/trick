# Program/library detection mirroring autoconf/configure.ac's non-LLVM,
# non-Python checks (tee, ld, flex, bison, curl, zip, gnuplot, swig, perl +
# modules, zlib, libxml2, threads, X11/Motif, java/maven) plus the small
# "other optional command line arguments" cluster (HDF5, GSL, civetweb).
# See autoconf/configure.ac and autoconf/m4/tr_*.m4 for the macros this
# replicates.

# ── Required simple tools (autoconf/configure.ac:111-124) ─────────────────
find_program(TEE_EXECUTABLE tee)
if(NOT TEE_EXECUTABLE)
    message(FATAL_ERROR "could not find tee")
endif()

find_program(LD_EXECUTABLE ld)
if(NOT LD_EXECUTABLE)
    message(FATAL_ERROR "could not find ld")
endif()

find_program(CURL_EXECUTABLE curl)
if(NOT CURL_EXECUTABLE)
    message(FATAL_ERROR "could not find curl")
endif()

find_program(ZIP_EXECUTABLE zip)
if(NOT ZIP_EXECUTABLE)
    message(FATAL_ERROR "Trick now requires zip but it could not be found. Please install zip. We recommend you use your OS package manager")
endif()

find_program(GNUPLOT_EXECUTABLE gnuplot)
if(NOT GNUPLOT_EXECUTABLE)
    message(NOTICE "could not find gnuplot")
endif()

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)
# AC_PROG_LEX([noyywrap]) (unlike AC_PATH_PROG(BISON,...)) stores the bare
# program name it matched, not a resolved path — match that exactly.
get_filename_component(FLEX_EXECUTABLE_NAME "${FLEX_EXECUTABLE}" NAME)

# ── Perl >= 5.14 + required modules (autoconf/configure.ac:121-122,195-196) ─
find_package(Perl 5.14 REQUIRED)
execute_process(
    COMMAND ${PERL_EXECUTABLE} -MText::Balanced -e 1
    RESULT_VARIABLE _tr_perl_text_balanced
    OUTPUT_QUIET ERROR_QUIET
)
if(NOT _tr_perl_text_balanced EQUAL 0)
    message(FATAL_ERROR "could not find perl modules Text::Balanced")
endif()
execute_process(
    COMMAND ${PERL_EXECUTABLE} -MDigest::MD5 -e 1
    RESULT_VARIABLE _tr_perl_digest_md5
    OUTPUT_QUIET ERROR_QUIET
)
if(NOT _tr_perl_digest_md5 EQUAL 0)
    message(FATAL_ERROR "could not find perl module Digest::MD5")
endif()

# ── SWIG >= 3.0 (autoconf/m4/tr_swig_bin.m4) ───────────────────────────────
find_package(SWIG 3.0 REQUIRED)

# ── zlib / libxml2 / threads ───────────────────────────────────────────────
find_package(ZLIB REQUIRED)

find_package(LibXml2 REQUIRED)
# AC_CHECK_LIB(xml2, main, ...) renders the literal flag, not a resolved path.
set(LIBXML "-lxml2")

find_package(Threads REQUIRED)
set(PTHREAD_CFLAGS "-pthread")
# AX_PTHREAD renders "-lpthread" on every Unix Trick supports, including
# Darwin (where CMAKE_THREAD_LIBS_INIT is empty because pthread symbols live
# in libSystem) — match its literal output rather than CMake's.
set(PTHREAD_LIBS "-lpthread")

# ── X11 / Motif (autoconf/m4/tr_xwindows.m4, tr_jsc_dirs.m4) ───────────────
find_package(X11)
if(X11_FOUND)
    set(USE_X_WINDOWS 1)
    if(X11_LIBRARY_DIRS)
        set(X_LIB_DIR "-L${X11_LIBRARY_DIRS}")
    elseif(X11_X11_LIB)
        get_filename_component(_tr_x11_libdir "${X11_X11_LIB}" DIRECTORY)
        set(X_LIB_DIR "-L${_tr_x11_libdir}")
    endif()
    if(NOT X11_Xt_FOUND)
        message(FATAL_ERROR "could not find libxt development headers")
    endif()

    # Motif is only required when the fermi-ware data_products tree exists.
    set(MOTIF_HOME "")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/trick_source/data_products/fermi-ware")
        if(EXISTS "/usr/include/Xm/Xm.h")
            set(MOTIF_HOME "/usr")
        elseif(EXISTS "/usr/local/include/Xm/Xm.h")
            set(MOTIF_HOME "/usr/local")
        elseif(EXISTS "/opt/homebrew/include/Xm/Xm.h")
            set(MOTIF_HOME "/opt/homebrew")
        else()
            message(FATAL_ERROR "could not find Xm/Xm.h")
        endif()
    endif()
else()
    set(USE_X_WINDOWS 0)
    set(X_LIB_DIR "")
    set(MOTIF_HOME "")
endif()

# ── Java / Maven (autoconf/m4/tr_java.m4), gated by TRICK_USE_JAVA ─────────
if(TRICK_USE_JAVA)
    set(USE_JAVA 1)
    find_package(Java 1.8 COMPONENTS Development REQUIRED)
    # AX_PROG_JAVA_CC(javac) only verifies "javac" is on PATH and stores the
    # literal candidate name, not a resolved path — match that exactly
    # rather than substituting Java_JAVAC_EXECUTABLE's absolute path.
    set(Java_JAVAC_EXECUTABLE "javac")
    if(NOT TRICK_OFFLINE)
        find_program(MVN_EXECUTABLE mvn)
        if(NOT MVN_EXECUTABLE)
            message(FATAL_ERROR "could not find maven")
        endif()
    else()
        set(MVN_EXECUTABLE "")
    endif()
else()
    set(USE_JAVA 0)
    set(Java_JAVAC_EXECUTABLE "")
    set(MVN_EXECUTABLE "")
endif()

# ── HDF5 (autoconf/m4/tr_hdf5_home.m4) ──────────────────────────────────────
if(HDF5_HOME)
    if(NOT EXISTS "${HDF5_HOME}/include/hdf5.h")
        message(FATAL_ERROR "could not find ${HDF5_HOME}/include/hdf5.h")
    endif()
elseif(EXISTS "/usr/include/hdf5.h")
    set(HDF5_HOME "/usr")
elseif(EXISTS "/opt/homebrew/include/hdf5.h")
    set(HDF5_HOME "/opt/homebrew")
else()
    set(HDF5_HOME "")
endif()

# ── GSL (autoconf/m4/tr_gsl_home.m4) ────────────────────────────────────────
if(GSL_HOME)
    if(NOT EXISTS "${GSL_HOME}/include/gsl")
        message(FATAL_ERROR "could not find ${GSL_HOME}/include/gsl")
    endif()
elseif(EXISTS "/usr/include/gsl/gsl_rng.h")
    set(GSL_HOME "/usr")
elseif(EXISTS "/opt/homebrew/include/gsl/gsl_rng.h")
    set(GSL_HOME "/opt/homebrew")
else()
    set(GSL_HOME "")
endif()

# ── civetweb (autoconf/m4/tr_civetweb_home.m4), gated by TRICK_CIVETWEB_HOME ─
if(TRICK_CIVETWEB_HOME)
    if(NOT EXISTS "${TRICK_CIVETWEB_HOME}/include/civetweb.h" OR
       NOT EXISTS "${TRICK_CIVETWEB_HOME}/lib/libcivetweb.a")
        message(FATAL_ERROR "Could not find all of the civetweb files.")
    endif()
    set(CIVETWEB_HOME "${TRICK_CIVETWEB_HOME}")
    set(USE_CIVETWEB 1)
elseif(EXISTS "/usr/include/civetweb.h")
    set(CIVETWEB_HOME "/usr")
    set(USE_CIVETWEB 1)
else()
    set(CIVETWEB_HOME "")
    set(USE_CIVETWEB 0)
endif()

# ── er7_utils, default enabled (autoconf/m4/tr_er7_utils.m4) ───────────────
option(TRICK_USE_ER7_UTILS "use er7_utils" ON)
if(TRICK_USE_ER7_UTILS)
    set(USE_ER7_UTILS 1)
else()
    set(USE_ER7_UTILS 0)
endif()

# ── Compiler full paths (autoconf/configure.ac:108-109) ────────────────────
# CMAKE_C/CXX_COMPILER are already resolved to absolute paths by CMake.
set(CC "${CMAKE_C_COMPILER}")
set(CXX "${CMAKE_CXX_COMPILER}")

# ── GCC version (autoconf/m4/tr_gcc_version.m4), empty for clang/mac ───────
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(GCC_VERSION "${CMAKE_CXX_COMPILER_VERSION}")
else()
    set(GCC_VERSION "")
endif()

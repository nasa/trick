dnl To generate a new top level configure script from this autoconf directory
dnl 1.  aclocal
dnl 2.  autoconf -o ../configure
dnl If you run into "configure.ac:15: error: possibly undefined macro: AC_MSG_ERROR",
dnl try:
dnl     autoreconf -fi

AC_INIT([Trick],[17+])
AC_LANG(C++)

AC_CONFIG_AUX_DIR([autoconf])

AC_DEFUN([AX_LLVM_HOME],[
    AC_ARG_WITH([llvm],
                AS_HELP_STRING([--with-llvm@<:@=DIR@:>@], [LLVM root directory]),
                [LLVM_HOME="$withval"
                 AC_PATH_PROG(LLVM_CONFIG, llvm-config, no-llvm-config, "$LLVM_HOME/bin")
                 AS_IF([test "$ac_cv_path_LLVM_CONFIG" = "no-llvm-config"],AC_MSG_ERROR([could not find llvm-config]),[])
                ],
                [
                 AC_PATH_PROG(LLVM_CONFIG, llvm-config, no-llvm-config, "/bin:/usr/bin:/usr/local/bin:/sw/bin:/usr/local/opt/llvm/bin")
                 AS_IF([test "$ac_cv_path_LLVM_CONFIG" = "no-llvm-config"],AC_MSG_ERROR([could not find llvm-config]),[])
                 LLVM_HOME=`$LLVM_CONFIG --prefix`
                ]
               )
])

AC_DEFUN([AX_HDF5_HOME],[
    AC_ARG_WITH([hdf5],
                AS_HELP_STRING([--with-hdf5@<:@=DIR@:>@], [HDF5 root directory]),
                [HDF5_HOME="$withval"
                 AS_IF([test "$HDF5_HOME" = "yes"],
                  AC_CHECK_HEADER(hdf5.h,[HDF5_HOME="/usr"],AC_MSG_ERROR([could not find hdf5.h])),
                  AS_IF([test "$HDF5_HOME" = "no"],[HDF5_HOME=""],
                   AC_CHECK_FILE([$HDF5_HOME/include/hdf5.h],
                                 [],
                                 AC_MSG_ERROR([could not find $HDF5_HOME/include/hdf5.h])
                                )
                  )
                )],
                 [
                   AC_CHECK_HEADER(hdf5.h, [HDF5_HOME="/usr"], [HDF5_HOME=""])
                 ]
               )
    AS_IF([test "$HDF5_HOME" != ""],
      [AC_CHECK_LIB(hdf5, main, [],AC_MSG_ERROR([could not find libhdf5]))]
      []
    )
    AC_SUBST([HDF5_HOME])
])

AC_DEFUN([AX_DMTCP_HOME],[
    AC_ARG_WITH([dmtcp],
                AS_HELP_STRING([--with-dmtcp@<:@=DIR@:>@], [DMTCP root directory]),
                [DMTCP_HOME="$withval"
                 AS_IF([test "$DMTCP_HOME" = "yes"],
                  AC_CHECK_HEADER(dmtcpaware.h,[DMTCP_HOME="/usr"],AC_MSG_ERROR([could not find dmtcpaware.h])),
                  AC_CHECK_FILE([$DMTCP_HOME/include/dmtcpaware.h],
                               [],
                               AC_MSG_ERROR([could not find $DMTCP_HOME/include/dmtcpaware.h])
                              )
                )],
                [DMTCP_HOME=""]
               )
    AC_SUBST([DMTCP_HOME])
])

AC_DEFUN([AX_TPRO_HOME],[
    AC_ARG_WITH([tpro],
                AS_HELP_STRING([--with-tpro@<:@=DIR@:>@], [TPRO root directory]),
                [TPRO_HOME="$withval"
                 AC_CHECK_FILE([$TPRO_HOME/linux/h/tpro.h],
                               [],
                               AC_MSG_ERROR([could not find $TPRO_HOME/linux/h/tpro.h])
                              )
                ],
                [TPRO_HOME=""]
               )
    AC_SUBST([TPRO_HOME])
])

AC_DEFUN([AX_BC635_HOME],[
    AC_ARG_WITH([bc635],
                AS_HELP_STRING([--with-bc635@<:@=DIR@:>@], [BC635 root directory]),
                [BC635_HOME="$withval"
                 AC_CHECK_FILE([$BC635_HOME/sample/bcuser.h],
                               [],
                               AC_MSG_ERROR([could not find $BC635_HOME/sample/bcuser.h])
                              )
                ],
                [BC635_HOME=""]
               )
    AC_SUBST([BC635_HOME])
])

AC_DEFUN([AX_GSL_HOME],[
    AC_ARG_WITH([gsl],
                AS_HELP_STRING([--with-gsl@<:@=DIR@:>@], [GSL root directory]),
                [GSL_HOME="$withval"
                 AS_IF([test "$GSL_HOME" = "yes"],
                  AC_CHECK_HEADER(gsl/gsl_rng.h,[GSL_HOME="/usr"],AC_MSG_ERROR([could not find gsl/gsl_rng.h])),
                  AS_IF([test "$GSL_HOME" = "no"],[GSL_HOME=""],
                    AC_CHECK_FILE([$GSL_HOME/include/gsl],
                                  [],
                                  AC_MSG_ERROR([could not find $GSL_HOME/include/gsl])
                                 )
                  )
                )],
                [AC_CHECK_HEADER(gsl/gsl_rng.h, [GSL_HOME="/usr"], [GSL_HOME=""])]
               )
    AS_IF([test "$GSL_HOME" != ""],
      [AC_CHECK_LIB(gsl, main, [],AC_MSG_ERROR([could not find libgsl]),[-lgslcblas])],
      []
    )
    AC_SUBST([GSL_HOME])
])

AC_DEFUN([AX_GTEST_HOME],[
    AC_ARG_WITH([gtest],
                AS_HELP_STRING([--with-gtest@<:@=DIR@:>@], [GTEST root directory]),
                [GTEST_HOME="$withval"
                 AS_IF([test "$GTEST_HOME" = "yes"],
                  AC_CHECK_HEADER(gtest/gtest.h,[GTEST_HOME="/usr"],AC_MSG_ERROR([could not find gtest/gtest.h])),
                  AS_IF([test "$GTEST_HOME" = "no"],[GTEST_HOME=""],
                    AC_CHECK_FILE([$GTEST_HOME/include/gtest],
                                  [],
                                  AC_MSG_ERROR([could not find $GTEST_HOME/include/gtest])
                                 )
                  )
                )],
                [AC_CHECK_HEADER(gtest/gtest.h, [GTEST_HOME="/usr"], [GTEST_HOME=""])]
               )
    AC_SUBST([GTEST_HOME])
])

AC_DEFUN([AX_SWIG_BIN],[
    AC_ARG_WITH([swig],
                [AS_HELP_STRING([--with-swig@<:@=DIR@:>@], [path of directory containing the SWIG executable.])],
                [
                  TEMP_PATH="${PATH}"
                  PATH="$withval:${PATH}"
                  AX_PKG_SWIG($1, [], [AC_MSG_ERROR([Trick requires SWIG version >= 2.0])])
                  PATH="${TEMP_PATH}"
                ],
                [AX_PKG_SWIG($1, [], [AC_MSG_ERROR([Trick requires SWIG version >= 2.0])])]
               )
])

AC_DEFUN([AX_UDUNITS_HOME],[
    AC_ARG_WITH([udunits],
                AS_HELP_STRING([--with-udunits@<:@=DIR@:>@], [UDUnits root directory]),
                [UDUNITS_HOME="$withval"],
                [UDUNITS_HOME=""]
               )
    AC_SUBST([UDUNITS_HOME])
])

dnl SOURCE: https://stackoverflow.com/a/59191148

dnl  NA_HELP_STRINGS(list1, help1[, list2, help2[, ... listN, helpN]])
dnl  **************************************************************************
dnl
dnl  Similar to `AS_HELP_STRING()`, but with support for multiple strings, each
dnl  one associated with one or more options
dnl
dnl  From: https://github.com/madmurphy/not-autotools
dnl
dnl  **************************************************************************
m4_define([NA_HELP_STRINGS],
    [m4_if(m4_count($1), [1],
        [m4_if([$#], [0], [], [$#], [1],
            [m4_text_wrap($1, [  ])],
            [AS_HELP_STRING(m4_normalize($1), [$2])m4_if([$#], [2], [], [m4_newline()NA_HELP_STRINGS(m4_shift2($@))])])],
        [m4_text_wrap(m4_argn(1, $1)[,], [  ])m4_newline()NA_HELP_STRINGS(m4_dquote(m4_shift($1))m4_if([$#], [1], [], [, m4_shift($@)]))])])



dnl test if want to prepend /usr/local/bin to PATH
dnl AC_ARG_WITH (package, help-string, [action-if-given], [action-if-not-given])
AC_DEFUN(
    [AX_PREPEND_PATH],
    [AC_ARG_WITH(
        [prepend-path],
        [NA_HELP_STRINGS(
[--without-prepend-path], [do not prepend to path (this is default)], [--with-prepend-path@<:@=DIR@:>@], [specify a directory to prepend to PATH (default is /usr/local/bin)])], 
        [AS_IF(
            [test "x${with_prepend_path}" = xyes],
            [PATH="/usr/local/bin:${PATH}"],
            [AS_IF(
                [test "x${with_prepend_path}" != xno],
                [PATH="${withval}:${PATH}"],
                []
            )]
        )],
        []
    )]
)


dnl test if we want to use java, default to yes
AC_DEFUN([AX_JAVA],[
    AC_ARG_ENABLE([java],
                  AS_HELP_STRING([--enable-java], [use java (default is yes)]),
                  AS_IF([test "x$enable_java" = xyes], [USE_JAVA="1"], [USE_JAVA="0"]),
                  [USE_JAVA="1"]
                 )
    AC_SUBST([USE_JAVA])
    AS_IF([test "$USE_JAVA" = "1"],[
        AC_PROG_AWK
        AX_PROG_JAVA_CC(javac)
        JAVA_VER=`$JAVA_CC -version 2>&1 | $ac_cv_path_PERL -ne 'print $& if /\d+(\.\d+)?/'`
        AC_MSG_CHECKING([$JAVA_CC version >= 1.8])
        AX_COMPARE_VERSION([$JAVA_VER],[ge],[1.8], [AC_MSG_RESULT([yes])],
         [AC_MSG_RESULT([no])
          AC_MSG_ERROR([Trick requires javac version >= 1.8])
         ])
        AS_IF([test "$TRICK_OFFLINE" = "0"], [
          AC_PATH_PROG(MVN, mvn, nomvn)
          AS_IF([test "$ac_cv_path_MVN" = "nomvn"],AC_MSG_ERROR([could not find maven]),[])
        ], [])
     ],[])
])

dnl test if we want to use er7_utils, default to yes
AC_DEFUN([AX_ER7_UTILS],[
    AC_ARG_ENABLE([er7utils],
                  AS_HELP_STRING([--enable-er7utils], [use er7_utils (default is yes)]),
                  AS_IF([test "x$enable_er7utils" = xyes], [USE_ER7_UTILS="1"], [USE_ER7_UTILS="0"]),
                  [USE_ER7_UTILS="1"]
                 )
    AC_SUBST([USE_ER7_UTILS])
])

dnl if fermi-ware directory exists, test for motif.
AC_DEFUN([AX_JSC_DIRS],[
    AC_CHECK_FILE([trick_source/data_products/fermi-ware],
      [
       AC_CHECK_HEADER(Xm/Xm.h, [MOTIF_HOME="/usr"],
        AC_CHECK_FILE(/usr/local/include/Xm/Xm.h, [MOTIF_HOME="/usr/local"],
         AC_CHECK_FILE(/sw/include/Xm/Xm.h, [MOTIF_HOME="/sw"],AC_MSG_ERROR([could not find Xm/Xm.h]))))
      ],
      [
       MOTIF_HOME=""
      ]
    )
    AC_SUBST([MOTIF_HOME])
])

AC_DEFUN([AX_GCC_VERSION], [
  GCC_VERSION=""
  AS_IF([test "x$GCC" = "xyes"],[
    AS_IF([test "x$ax_gcc_version_option" != "xno"],[
      AC_CACHE_CHECK([gcc version],[ax_cv_gcc_version],[
        ax_cv_gcc_version="`$CC -dumpversion`"
        AS_IF([test "x$ax_cv_gcc_version" = "x"],[
          ax_cv_gcc_version=""
        ])
      ])
      GCC_VERSION=$ax_cv_gcc_version
    ])
  ])
  AC_SUBST([GCC_VERSION])
])

dnl get the host and build.
AC_CANONICAL_HOST
AC_CANONICAL_BUILD

dnl AC_CANONICAL_HOST provides platform.  MacOSX is diverging... have to do special things
ON_MAC=no
case "${host_os}" in
    darwin*)
        ON_MAC=yes
        ;;
    *)
        ;;
esac


dnl add extra paths to find xml headers and X headers on the mac.
AS_IF([test "$ON_MAC" = "yes"],
[
 AC_PATH_PROG(XCRUN, xcrun, noxcrun)
 AS_IF([test "$ac_cv_path_XCRUN" = "norun"],AC_MSG_ERROR([could not find xcrun - install Xcode command line tools]),[])
 XCODE_SDK_PATH=`$XCRUN --show-sdk-path`
 XTRAINCPATHS="-I/usr/X11/include -I${XCODE_SDK_PATH}/usr/include -I${XCODE_SDK_PATH}/usr/include/libxml2"
],
[
 XTRAINCPATHS="-I/usr/include/libxml2/"
])
CFLAGS="$CFLAGS $XTRAINCPATHS"
CPPFLAGS="$CPPFLAGS $XTRAINCPATHS"

dnl look for X windows libraries and headers we need to compile
dnl these checks must be done before we force 32bit mode.
AC_PATH_X
AS_IF([test "$no_x" = "yes"],
  [
    USE_X_WINDOWS=0
  ],
  [
    USE_X_WINDOWS=1
    AS_IF([test "x$x_includes" = "x"],[],[X_INCLUDE_DIR=-I$x_includes])
    AS_IF([test "x$x_libraries" = "x"],[],[X_LIB_DIR=-L$x_libraries])
    AC_CHECK_HEADER([X11/Intrinsic.h],[],AC_MSG_ERROR([could not find libxt development headers]))
    AX_JSC_DIRS([])
  ]
)
AC_SUBST([USE_X_WINDOWS])
AC_SUBST([X_LIB_DIR])

dnl TODO Need to check 64bit only!
AC_CHECK_LIB(xml2, main,
    [LIBXML=-lxml2
     AC_SUBST([LIBXML])]
)
AC_CHECK_HEADER([libxml/tree.h],[],AC_MSG_ERROR([could not find libxml development headers]))

# If 32bit is specified, set some compilation flags.
AC_ARG_ENABLE([32bit],
  AS_HELP_STRING([--enable-32bit], [Force Trick and sims to compile 32bit]),
  AS_IF([test "x$enable_32bit" = xyes],
  [
   TRICK_FORCE_32BIT=1
   CFLAGS="${CFLAGS} -m32"
   CXXFLAGS="${CXXFLAGS} -m32"
   LDFLAGS="${LDFLAGS} -m32"
   ARCH="i686"
  ], [TRICK_FORCE_32BIT=0]),
  [TRICK_FORCE_32BIT=0]
)
AC_SUBST([TRICK_FORCE_32BIT])

# If offline is specified, set some compilation flags
AC_ARG_ENABLE([offline],
  AS_HELP_STRING([--enable-offline], [Compile Trick in offline mode. Un-tar the trick-offline directory in TRICK_HOME to use this feature]),
  AS_IF([test "x$enable_offline" = xyes],
    [
      [TRICK_OFFLINE="1"]
      AC_CHECK_FILE([trick-offline], [], AC_MSG_ERROR([Offline mode requires an uncompressed directory named "trick-offline" in TRICK_HOME]))
    ], 
    [TRICK_OFFLINE="0"]
  ),
  [TRICK_OFFLINE="0"]
)
AC_SUBST([TRICK_OFFLINE])

# If offline is specified, set some compilation flags
AC_ARG_ENABLE([offline],
  AS_HELP_STRING([--enable-offline], [Compile Trick in offline mode. Un-tar the trick-offline directory in TRICK_HOME to use this feature]),
  AS_IF([test "x$enable_offline" = xyes],
    [TRICK_OFFLINE="1"], 
    [TRICK_OFFLINE="0"]
  ),
  [TRICK_OFFLINE="0"]
)
AC_SUBST([TRICK_OFFLINE])

AC_ARG_ENABLE([civet],
  AS_HELP_STRING([--enable-civet], [Compile Trick with webserver capabilites.]),
  AS_IF([test "x$enable_civet" = xyes],
    [
    TRICK_DISABLE_CIVET="0"
    ], 
    [TRICK_DISABLE_CIVET="1"]
  ),
  [TRICK_DISABLE_CIVET="0"]
)
AC_SUBST([TRICK_DISABLE_CIVET])

# If offline is specified, set some compilation flags
AC_ARG_ENABLE([offline],
  AS_HELP_STRING([--enable-offline], [Compile Trick in offline mode. Un-tar the trick-offline directory in TRICK_HOME to use this feature]),
  AS_IF([test "x$enable_offline" = xyes],
    [TRICK_DISABLE_CIVET="1"], 
  ),
)
AC_SUBST([TRICK_OFFLINE])

dnl look for programs we need to compile and run
AC_PROG_AWK
AC_PROG_CC
AC_PROG_CXX

dnl do not test gcc version on mac.  clang as gcc will return version and it's not >= 4.8
AS_IF([test "$ON_MAC" = "no"],[AX_GCC_VERSION],[])
dnl if the compiler is gcc, test for gcc >= 4.8
AS_IF([test "x$GCC_VERSION" = "x"],[],
 [AC_MSG_CHECKING([gcc version >= 4.8])
  AX_COMPARE_VERSION([$GCC_VERSION],[ge],[4.8], [AC_MSG_RESULT([yes])],
   [AC_MSG_RESULT([no])
    AC_MSG_ERROR([Trick requires gcc version >= 4.8])
   ])
])
dnl Save the full path of the compiler
CC=`which $CC`
CXX=`which $CXX`

AC_PATH_PROG(TEE, tee, notee)
AS_IF([test "$ac_cv_path_TEE" = "notee"],AC_MSG_ERROR([could not find tee]),[])
AC_PATH_PROG(LD, ld, nold)
AS_IF([test "$ac_cv_path_LD" = "nold"],AC_MSG_ERROR([could not find ld]),[])
AC_PROG_LEX
AS_IF([test "x$LEX" = "x:"],AC_MSG_ERROR([could not find flex]),[])
AC_PATH_PROG(BISON, bison, nobison)
AS_IF([test "$ac_cv_path_BISON" = "nobison"],AC_MSG_ERROR([could not find bison]),[])
AC_PATH_PROG(CURL, curl, nocurl)
AS_IF([test "$ac_cv_path_CURL" = "nocurl"],AC_MSG_ERROR([could not find curl]),[])
AC_PATH_PROG(PERL, perl)
AX_PROG_PERL_VERSION([5.14.0], [], [AC_MSG_ERROR([Trick requires perl version >= 5.14.0])])

AC_ARG_WITH(
  [python],
  AS_HELP_STRING(
    [--with-python@<:@=DIR@:>@], 
    [python root directory],
  ),
  [
    PYTHON_HOME="$withval"
    AC_PATH_PROGS(PYTHON, python${PYTHON_VERSION} python python3, nopython, "$PYTHON_HOME")
    AS_IF(
      [test "$ac_cv_path_PYTHON" = "nopython"],
      AC_MSG_ERROR([could not find python<version> python or python3 in the specefied path. Please install the python development package]),
      []
    )
    PYTHON_MAJORMINOR=`${PYTHON} -c 'import sys; print(str(sys.version_info[[0]])+"."+str(sys.version_info[[1]]))'`
    PYTHON_MAJOR=`${PYTHON} -c 'import sys; print(str(sys.version_info[[0]]))'`
    AC_PATH_PROGS(PYTHON_CONFIG, python${PYTHON_MAJORMINOR}-config python${PYTHON_MAJOR}-config python${PYTHON_VERSION}-config python-config, nopython-config, "$PYTHON_HOME")
    AS_IF(
      [test "$ac_cv_path_PYTHON_CONFIG" = "nopython-config"],
      AC_MSG_ERROR([could not find python<major>-config python<major.minor>-config or python-config in the specified path. Please install the python development package]),
      []
    )
  ],
  [
    AC_PATH_PROGS(PYTHON, python${PYTHON_VERSION} python python3, nopython)
    AS_IF(
      [test "$ac_cv_path_PYTHON" = "nopython"],
      AC_MSG_ERROR([could not find python<version> python or python3. Please install the python development package]),
      []
    )
    PYTHON_MAJORMINOR=`${PYTHON} -c 'import sys; print(str(sys.version_info[[0]])+"."+str(sys.version_info[[1]]))'`
    PYTHON_MAJOR=`${PYTHON} -c 'import sys; print(str(sys.version_info[[0]]))'`
    AC_PATH_PROGS(PYTHON_CONFIG, python${PYTHON_MAJORMINOR}-config python${PYTHON_MAJOR}-config python${PYTHON_VERSION}-config python-config, nopython-config)
    AS_IF(
      [test "$ac_cv_path_PYTHON_CONFIG" = "nopython-config"],
      AC_MSG_ERROR([could not find python<major>-config python<major.minor>-config or python-config. Please install the python development package]),
      []
    )  
  ]
)

PYTHON_LIBS_COMMAND="${PYTHON_CONFIG} --ldflags --libs"

AX_COMPARE_VERSION(
  [${PYTHON_MAJORMINOR}],
  [ge],
  [3.8],
  [PYTHON_LIBS_COMMAND="${PYTHON_LIBS_COMMAND} --embed"]
)

PYTHON_CPPFLAGS=`${PYTHON_CONFIG} --includes | sed 's/-I/-isystem/g'`
PYTHON_LIBS=`${PYTHON_LIBS_COMMAND} | tr '\r\n' ' '`

AC_SUBST([PYTHON_CPPFLAGS])
AC_SUBST([PYTHON_LIBS])
AC_SUBST([PYTHON_EXTRA_LIBS])
AX_PREPEND_PATH
AC_PATH_PROG(GNUPLOT, gnuplot, nognuplot)
AS_IF([test "$ac_cv_path_GNUPLOT" = "nognuplot"],AC_MSG_NOTICE([could not find gnuplot]),[])
AX_SWIG_BIN([2.0])
AX_JAVA

AX_PTHREAD()
AX_PROG_PERL_MODULES( Text::Balanced ,[],AC_MSG_ERROR([could not find perl modules Text::Balanced]))
AX_PROG_PERL_MODULES( Digest::MD5,[],AC_MSG_ERROR([could not find perl module Digest::MD5]))

dnl process the optional --with-llvm command line arguments
AX_LLVM_HOME([])
LLVM_LIB_DIR=`$LLVM_CONFIG --libdir`
LLVM_BIN_DIR=`$LLVM_CONFIG --bindir`
LLVM_INCLUDE_DIR=`$LLVM_CONFIG --includedir`
AC_CHECK_FILE([$LLVM_INCLUDE_DIR/clang/Basic/LLVM.h],[],AC_MSG_ERROR([could not find clang development headers]))
AC_CHECK_FILE([$LLVM_LIB_DIR/libLLVMSupport.a],
  [],
  AC_MSG_ERROR([could not find clang library files in $LLVM_LIB_DIR])
)
AC_PATH_PROG(CLANG, clang, noclang, "$LLVM_BIN_DIR:/bin:/usr/bin:/usr/local/bin:/sw/bin")
AS_IF([test "$ac_cv_path_CLANG" = "noclang"],AC_MSG_ERROR([could not find clang]),[])
AC_SUBST([LLVM_HOME])

OLD_CLANG_LIBS="-lclangFrontend -lclangDriver -lclangSerialization -lclangParse -lclangSema -lclangAnalysis -lclangEdit -lclangAST -lclangLex -lclangBasic"
NEW_CLANG_LIBS="-lclang-cpp"
AC_CHECK_FILE([$LLVM_LIB_DIR/libclangFrontend.a],
  [
    ICG_CLANGLIBS="$OLD_CLANG_LIBS"
  ],
  AC_CHECK_FILE([$LLVM_LIB_DIR/libclangFrontend.so],
    [
      ICG_CLANGLIBS="$OLD_CLANG_LIBS"
    ],
    AC_CHECK_FILE([$LLVM_LIB_DIR/libclang-cpp.a],
      [
        ICG_CLANGLIBS="$NEW_CLANG_LIBS"
      ],
      AC_CHECK_FILE([$LLVM_LIB_DIR/libclang-cpp.so],
        [
          ICG_CLANGLIBS="$NEW_CLANG_LIBS"
        ],
        AC_MSG_ERROR([could not find clang libs in LLVM library: "$LLVM_LIB_DIR"])
      )
    )
  )
)

AC_SUBST([ICG_CLANGLIBS])


AC_DEFUN([AX_CLANG_VERSION], [
  CLANG_VERSION=""
  ax_cv_clang_version="`$CLANG --version | grep "version" | sed "s/.*version \([0-9]*\.[0-9]*\.[0-9]*\).*/\1/"`"
  AS_IF([test "x$ax_cv_clang_version" = "x"],[
    ax_cv_clang_version=""
  ])
  CLANG_VERSION=$ax_cv_clang_version
  AC_SUBST([CLANG_VERSION])
])
AX_CLANG_VERSION
dnl if llvm/clang, test for version >= 3.4.2
AS_IF([test "x$CLANG_VERSION" = "x"],[],
 [AC_MSG_CHECKING([clang version >= 3.4.2])
  AX_COMPARE_VERSION([$CLANG_VERSION],[ge],[3.4.2], [AC_MSG_RESULT([yes])],
   [AC_MSG_RESULT([no])
    AC_MSG_ERROR([Trick requires llvm/clang version >= 3.4.2])
   ])
])

AX_CHECK_ZLIB([],AC_MSG_ERROR([could not find zlib]))

dnl look for udunits in /usr/include and /usr/include/udunits2
AX_UDUNITS_HOME([])
AS_IF([test "$UDUNITS_HOME" = ""],
 [
  AC_CHECK_HEADER([udunits2.h],
   [UDUNITS_INCLUDES=],
   [AC_CHECK_HEADER([udunits2/udunits2.h],[UDUNITS_INCLUDES=-I/usr/include/udunits2],AC_MSG_ERROR([could not find udunits development headers]))]
  )
  UDUNITS_LDFLAGS=-ludunits2
 ],
 [
  UDUNITS_EXCLUDE=$UDUNITS_HOME
  AC_CHECK_FILE([$UDUNITS_HOME/include/udunits2.h],
   [UDUNITS_INCLUDES=-I$UDUNITS_HOME/include
    UDUNITS_LDFLAGS="-Wl,-rpath,$UDUNITS_HOME/lib -L$UDUNITS_HOME/lib -ludunits2"
   ],
   [AC_CHECK_FILE([$UDUNITS_HOME/lib/udunits2.h],
    [UDUNITS_INCLUDES=-I$UDUNITS_HOME/lib
     UDUNITS_LDFLAGS="-Wl,-rpath,$UDUNITS_HOME/lib -L$UDUNITS_HOME/lib -ludunits2"
    ],
    AC_MSG_ERROR([could not find udunits2]))
   ]
  )
  CFLAGS="${CFLAGS} ${UDUNITS_INCLUDES}"
  LDFLAGS="${LDFLAGS} ${UDUNITS_LDFLAGS}"
 ]
)
AC_CHECK_LIB(udunits2, main, [],AC_MSG_ERROR([could not find libudunits]))
AC_SUBST([UDUNITS_INCLUDES])
AC_SUBST([UDUNITS_LDFLAGS])
AC_SUBST([UDUNITS_EXCLUDE])

dnl process the other optional command line arguments
AX_HDF5_HOME([])
AX_DMTCP_HOME([])
AX_TPRO_HOME([])
AX_BC635_HOME([])
AX_GSL_HOME([])
AX_GTEST_HOME([])
AX_ER7_UTILS([])

AC_OUTPUT([share/trick/makefiles/config_user.mk])

printf "\033@<:@32mconfigure script successfully completed\033@<:@0m\n"

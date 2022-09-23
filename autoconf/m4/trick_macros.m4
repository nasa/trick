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
      [
      LDFLAGS="${LDFLAGS} -L${HDF5_HOME}/lib"
      AC_CHECK_LIB(hdf5, main, [],AC_MSG_ERROR([could not find libhdf5]))
      ]
      []
    )
    AC_SUBST([HDF5_HOME])
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
      [
      LDFLAGS="${LDFLAGS} -L${GSL_HOME}/lib"
      AC_CHECK_LIB(gsl, main, [],AC_MSG_ERROR([could not find libgsl]),[-lgslcblas])
      ],
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

AC_DEFUN([AX_CIVETWEB_HOME],[
    AC_ARG_WITH([civetweb],
                AS_HELP_STRING([--with-civetweb@<:@=DIR@:>@],
                  [CIVETWEB root directory]),
                # --with option was provided.
                [CIVETWEB_HOME="$withval"
                 USE_CIVETWEB="0"
                # check whether directory arg was also provided.
                 AS_IF([test "$CIVETWEB_HOME" = "yes"],
                   AC_CHECK_HEADER(civetweb.h,
                     [CIVETWEB_HOME="/usr"; USE_CIVETWEB="1"],
                     AC_MSG_ERROR([could not find civetweb.h])),
                   # else check whether --without-civet or --with-civet=no specified.
                   AS_IF([test "$CIVETWEB_HOME" = "no"],
                     [CIVETWEB_HOME=""],
                         # else --with-civet was provided with a directory path.
                         AC_CHECK_FILES([$CIVETWEB_HOME/include/civetweb.h $CIVETWEB_HOME/lib/libcivetweb.a],
                           [USE_CIVETWEB="1"],
                           AC_MSG_ERROR([Could not find all of the civetweb files.]))
                 ))
                ],
                # --with option not provided.
                [AC_CHECK_HEADER(civetweb.h,
                   [CIVETWEB_HOME="/usr"; USE_CIVETWEB="1"],
                   [CIVETWEB_HOME=""; USE_CIVETWEB="0"])
                ]
               )
    AC_SUBST([CIVETWEB_HOME])
    AC_SUBST([USE_CIVETWEB])
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
        ax_cv_gcc_version="`$CC -dumpfullversion -dumpversion`"
        AS_IF([test "x$ax_cv_gcc_version" = "x"],[
          ax_cv_gcc_version=""
        ])
      ])
      GCC_VERSION=$ax_cv_gcc_version
    ])
  ])
  AC_SUBST([GCC_VERSION])
])

AC_DEFUN([AX_CLANG_VERSION], [
  CLANG_VERSION=""
  ax_cv_clang_version="`$CLANG --version | grep "version" | sed "s/.*version \([0-9]*\.[0-9]*\.[0-9]*\).*/\1/"`"
  AS_IF([test "x$ax_cv_clang_version" = "x"],[
    ax_cv_clang_version=""
  ])
  CLANG_VERSION=$ax_cv_clang_version
  AC_SUBST([CLANG_VERSION])
])
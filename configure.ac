AC_INIT([Trick],[16.0])
AC_LANG(C++)

AC_DEFUN([AX_LLVM_HOME],[
    AC_ARG_WITH([llvm],
                AS_HELP_STRING([--with-llvm@<:@=DIR@:>@], [LLVM root directory]),
                [LLVM_HOME="$withval"],
                [LLVM_HOME="/usr"]
               )
    AC_CHECK_FILE([$LLVM_HOME/bin/llvm-config],
                  [AC_SUBST([LLVM_HOME])],
                  AC_MSG_ERROR([could not find $LLVM_HOME/bin/llvm-config])
                 )
])

AC_DEFUN([AX_HDF5_HOME],[
    AC_ARG_WITH([hdf5],
                AS_HELP_STRING([--with-hdf5@<:@=DIR@:>@], [HDF5 root directory]),
                [HDF5_HOME="$withval"
                 AC_CHECK_FILE([$HDF5_HOME/include/hdf5.h],
                               [],
                               AC_MSG_ERROR([could not find $HDF5_HOME/include/hdf5.h])
                              )
                ],
                [HDF5_HOME=""]
               )
    AC_SUBST([HDF5_HOME])
])

AC_DEFUN([AX_DMTCP_HOME],[
    AC_ARG_WITH([dmtcp],
                AS_HELP_STRING([--with-dmtcp@<:@=DIR@:>@], [DMTCP root directory]),
                [DMTCP_HOME="$withval"
                 AC_CHECK_FILE([$DMTCP_HOME/include/dmtcpaware.h],
                               [],
                               AC_MSG_ERROR([could not find $DMTCP_HOME/include/dmtcpaware.h])
                              )
                ],
                [DMTCP_HOME=""]
               )
    AC_SUBST([DMTCP_HOME])
])

AC_DEFUN([AX_TPRO_HOME],[
    AC_ARG_WITH([tpro],
                AS_HELP_STRING([--with-tpro@<:@=DIR@:>@], [TPRO root directory]),
                [TPROCTE_HOME="$withval"
                 AC_CHECK_FILE([$TPRO_HOME/h/tpro.h],
                               [],
                               AC_MSG_ERROR([could not find $TPRO_HOME/h/tpro.h])
                              )
                ],
                [TPROCTE_HOME=""]
               )
    AC_SUBST([TPROCTE_HOME])
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
                 AC_CHECK_FILE([$GSL_HOME/include/gsl],
                               [],
                               AC_MSG_ERROR([could not find $GSL_HOME/sample/bcuser.h])
                              )
                ],
                [GSL_HOME=""]
               )
    AC_SUBST([GSL_HOME])
])

AC_DEFUN([AX_GTEST_HOME],[
    AC_ARG_WITH([gtest],
                AS_HELP_STRING([--with-gtest@<:@=DIR@:>@], [GTEST root directory]),
                [GTEST_HOME="$withval"
                 AC_CHECK_FILE([$GTEST_HOME/include/gtest],
                               [],
                               AC_MSG_ERROR([could not find $GTEST_HOME/sample/bcuser.h])
                              )
                ],
                [GTEST_HOME=""]
               )
    AC_SUBST([GTEST_HOME])
])

AC_PROG_CC
AC_PROG_CXX
AC_PROG_AWK
AC_PROG_SED
AC_PROG_LEX
AC_PATH_PROG(BISON, bison, bison)
AC_PATH_PROG(PERL, perl, perl)
AX_PKG_SWIG(1.3.29, [], [ AC_MSG_ERROR([SWIG is required to build..]) ])
AX_PROG_JAVAC

AC_CHECK_LIB(xml2, main,
    [LIBXML=-lxml2
     AC_SUBST([LIBXML])]
)

AX_LLVM_HOME([])
AX_HDF5_HOME([])
AX_DMTCP_HOME([])
AX_TPRO_HOME([])
AX_BC635_HOME([])
AX_GSL_HOME([])
AX_GTEST_HOME([])

AX_PYTHON_DEVEL([>='2.5'])
AC_CHECK_HEADER(Xm/Xm.h, [MOTIF_HOME="/usr"], AC_CHECK_FILE(/sw/include/Xm/Xm.h, [MOTIF_HOME="/sw"]))
AC_SUBST([MOTIF_HOME])


AC_OUTPUT([share/trick/makefiles/config_user.mk])


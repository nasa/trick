

AC_DEFUN([TR_SWIG_BIN],[
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
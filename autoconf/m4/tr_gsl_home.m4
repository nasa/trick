

AC_DEFUN([TR_GSL_HOME],[
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

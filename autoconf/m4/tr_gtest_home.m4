

AC_DEFUN([TR_GTEST_HOME],[
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
AC_DEFUN([TR_GTEST],[
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
                [AC_CHECK_HEADER(gtest/gtest.h, [GTEST_HOME="/usr"], [AC_CHECK_FILE([/opt/homebrew/include/gtest/gtest.h], [GTEST_HOME="/opt/homebrew"],[GTEST_HOME=""])])]
               )

    dnl Check gtest version using pkg-config if available
    GTEST_CXXSTD=""
    AS_IF([test -n "$GTEST_HOME"], [
        PKG_CHECK_EXISTS([gtest], [
            GTEST_VERSION=`$PKG_CONFIG --modversion gtest 2>/dev/null`
            AS_IF([test -n "$GTEST_VERSION"], [
                AC_MSG_CHECKING([gtest version])
                AC_MSG_RESULT([$GTEST_VERSION])

                dnl Extract major and minor version numbers
                GTEST_MAJOR=`echo $GTEST_VERSION | cut -d. -f1`
                GTEST_MINOR=`echo $GTEST_VERSION | cut -d. -f2`

                dnl Set C++ standard based on gtest version
                dnl gtest >= 1.17 requires C++17
                AS_IF([test "$GTEST_MAJOR" -gt 1 -o \( "$GTEST_MAJOR" -eq 1 -a "$GTEST_MINOR" -ge 17 \)], [
                    GTEST_CXXSTD="-std=c++17"
                    AC_MSG_NOTICE([gtest >= 1.17.0 detected, setting C++ standard to C++17])
                ], [
                    dnl gtest < 1.17 uses C++14
                    GTEST_CXXSTD="-std=c++14"
                    AC_MSG_NOTICE([gtest < 1.17.0 detected, setting C++ standard to C++14])
                ])
            ], [
                AC_MSG_WARN([Could not determine gtest version via pkg-config])
            ])
        ], [
            AC_MSG_WARN([pkg-config gtest not found, version detection skipped])
        ])
    ])

    AC_SUBST([GTEST_HOME])
    AC_SUBST([GTEST_VERSION])
    AC_SUBST([GTEST_CXXSTD])
])

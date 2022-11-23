

AC_DEFUN([TR_CIVETWEB_HOME],[
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



dnl test if we want to use er7_utils, default to yes
AC_DEFUN([TR_ER7_UTILS],[
    AC_ARG_ENABLE([er7utils],
                  AS_HELP_STRING([--enable-er7utils], [use er7_utils (default is yes)]),
                  AS_IF([test "x$enable_er7utils" = xyes], [USE_ER7_UTILS="1"], [USE_ER7_UTILS="0"]),
                  [USE_ER7_UTILS="1"]
                 )
    AC_SUBST([USE_ER7_UTILS])
])
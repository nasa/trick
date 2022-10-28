

dnl test if we want to use java, default to yes
AC_DEFUN([TR_JAVA],[
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


dnl look for X windows libraries and headers we need to compile
AC_DEFUN([TR_XWINDOWS], [
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
	    TR_JSC_DIRS([])
	  ]
	)
	AC_SUBST([USE_X_WINDOWS])
	AC_SUBST([X_LIB_DIR])
])
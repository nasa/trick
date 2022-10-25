

AC_DEFUN([TR_CLANG_VERSION], [
  CLANG_VERSION=""
  ax_cv_clang_version="`$CLANG --version | grep "version" | sed "s/.*version \([0-9]*\.[0-9]*\.[0-9]*\).*/\1/"`"
  AS_IF([test "x$ax_cv_clang_version" = "x"],[
    ax_cv_clang_version=""
  ])
  CLANG_VERSION=$ax_cv_clang_version
  AC_SUBST([CLANG_VERSION])
])
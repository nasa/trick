

dnl test if want to prepend /usr/local/bin to PATH
dnl AC_ARG_WITH (package, help-string, [action-if-given], [action-if-not-given])
AC_DEFUN(
    [TR_PREPEND_PATH],
    [AC_ARG_WITH(
        [prepend-path],
        [NA_HELP_STRINGS(
[--without-prepend-path], [do not prepend to path (this is default)], [--with-prepend-path@<:@=DIR@:>@], [specify a directory to prepend to PATH (default is /usr/local/bin)])], 
        [AS_IF(
            [test "x${with_prepend_path}" = xyes],
            [PATH="/usr/local/bin:${PATH}"],
            [AS_IF(
                [test "x${with_prepend_path}" != xno],
                [PATH="${withval}:${PATH}"],
                []
            )]
        )],
        []
    )]
)
dnl ADDITIONAL INFORMATION: https://stackoverflow.com/a/59191148
dnl AUTHOR: madmurphy [GitHub, Stack Overflow]

dnl  NA_HELP_STRINGS(list1, help1[, list2, help2[, ... listN, helpN]])
dnl  **************************************************************************
dnl
dnl  Similar to `AS_HELP_STRING()`, but with support for multiple strings, each
dnl  one associated with one or more options
dnl
dnl  From: https://github.com/madmurphy/not-autotools
dnl
dnl  **************************************************************************
AC_DEFUN([NA_HELP_STRINGS],
    [m4_if(m4_count($1), [1],
        [m4_if([$#], [0], [], [$#], [1],
            [m4_text_wrap($1, [  ])],
            [AS_HELP_STRING(m4_normalize($1), [$2])m4_if([$#], [2], [], [m4_newline()NA_HELP_STRINGS(m4_shift2($@))])])],
        [m4_text_wrap(m4_argn(1, $1)[,], [  ])m4_newline()NA_HELP_STRINGS(m4_dquote(m4_shift($1))m4_if([$#], [1], [], [, m4_shift($@)]))])])

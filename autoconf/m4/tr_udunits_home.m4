

AC_DEFUN([TR_UDUNITS_HOME],[
    AC_ARG_WITH([udunits],
                AS_HELP_STRING([--with-udunits@<:@=DIR@:>@], [UDUnits root directory]),
                [UDUNITS_HOME="$withval"],
                AC_CHECK_FILE(/opt/homebrew/include/udunits2.h, [UDUNITS_HOME="/opt/homebrew"],AC_MSG_ERROR([could not find udunits2.h]))
                )
    AC_SUBST([UDUNITS_HOME])
])

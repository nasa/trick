

AC_DEFUN([TR_UDUNITS_HOME],[
    AC_ARG_WITH([udunits],
                AS_HELP_STRING([--with-udunits@<:@=DIR@:>@], [UDUnits root directory]),
                [UDUNITS_HOME="$withval"],
                [UDUNITS_HOME=""]
               )
    AC_SUBST([UDUNITS_HOME])
])
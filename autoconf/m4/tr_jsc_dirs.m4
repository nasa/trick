

dnl if fermi-ware directory exists, test for motif.
AC_DEFUN([TR_JSC_DIRS],[
    AC_CHECK_FILE([trick_source/data_products/fermi-ware],
      [
       AC_CHECK_HEADER(Xm/Xm.h, [MOTIF_HOME="/usr"],
        AC_CHECK_FILE(/usr/local/include/Xm/Xm.h, [MOTIF_HOME="/usr/local"],
         AC_CHECK_FILE(/sw/include/Xm/Xm.h, [MOTIF_HOME="/sw"],AC_MSG_ERROR([could not find Xm/Xm.h]))))
      ],
      [
       MOTIF_HOME=""
      ]
    )
    AC_SUBST([MOTIF_HOME])
])
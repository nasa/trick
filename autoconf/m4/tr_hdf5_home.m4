

AC_DEFUN([TR_HDF5_HOME],[
    AC_ARG_WITH([hdf5],
                AS_HELP_STRING([--with-hdf5@<:@=DIR@:>@], [HDF5 root directory]),
                [HDF5_HOME="$withval"
                 AS_IF([test "$HDF5_HOME" = "yes"],
                  AC_CHECK_HEADER(hdf5.h,[HDF5_HOME="/usr"],AC_MSG_ERROR([could not find hdf5.h])),
                  AS_IF([test "$HDF5_HOME" = "no"],[HDF5_HOME=""],
                   AC_CHECK_FILE([$HDF5_HOME/include/hdf5.h],
                                 [],
                                 AC_MSG_ERROR([could not find $HDF5_HOME/include/hdf5.h])
                                )
                  )
                )],
                 [
                   AC_CHECK_HEADER(hdf5.h, [HDF5_HOME="/usr"], [HDF5_HOME=""])
                 ]
               )
    AS_IF([test "$HDF5_HOME" != ""],
      [
      LDFLAGS="${LDFLAGS} -L${HDF5_HOME}/lib"
      AC_CHECK_LIB(hdf5, main, [],AC_MSG_ERROR([could not find libhdf5]))
      ]
      []
    )
    AC_SUBST([HDF5_HOME])
])
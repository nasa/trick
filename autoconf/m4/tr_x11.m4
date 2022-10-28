
dnl find xml headers based on host type
AC_DEFUN([TR_X11], [
    AS_IF([test "$ON_MAC" = "yes"],
    [
     AC_PATH_PROG(XCRUN, xcrun, noxcrun)
     AS_IF([test "$ac_cv_path_XCRUN" = "norun"],AC_MSG_ERROR([could not find xcrun - install Xcode command line tools]),[])
     XCODE_SDK_PATH=`$XCRUN --show-sdk-path`
     XTRAINCPATHS="-I/usr/X11/include -I${XCODE_SDK_PATH}/usr/include -I${XCODE_SDK_PATH}/usr/include/libxml2"
    ],
    [
     XTRAINCPATHS="-I/usr/include/libxml2/"
    ])
    CFLAGS="$CFLAGS $XTRAINCPATHS"
    CPPFLAGS="$CPPFLAGS $XTRAINCPATHS"
])
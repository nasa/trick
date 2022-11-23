
dnl Set ON_MAC=yes if we are on a mac (darwin host)
AC_DEFUN([TR_MAC_DARWIN], [
    dnl AC_CANONICAL_HOST provides platform.  MacOSX is diverging... have to do special things
    ON_MAC=no
    case "${host_os}" in
        darwin*)
            ON_MAC=yes
            ;;
        *)
            ;;
    esac
])
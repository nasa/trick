# Trick.profile
# This provides the default Trick environment for Trick users.
# Users may override any variable within their .Trick_user_profile

#################################################################
# Is Trick installed where TRICK_HOME is?
#################################################################
PERL=`which perl`
if [ ! -f ${TRICK_HOME}/bin/gte ] ;  then
    echo "${TRICK_HOME}/bin/gte doesn't exist.. exiting .Trick_profile\!\!"
elif [ ! -f "${PERL}" ] ;  then
    if [  -f /usr/bin/perl ] ;  then
        PERL="/usr/bin/perl"
    else
        echo "${PERL} Perl doesn't exist... Exiting .Trick_profile\!\!"
    fi
fi

# Fix locale settings to avoid perl problems on MacOS
OS_NAME=`uname -s`
if [ $OS_NAME = "Darwin" ] ; then
    # if the LC_ALL environment variable doesn't exist
    if [ -z "$LC_ALL" ] ; then
        # Prevent this error-> "perl: warning: Setting locale failed."
        LC_ALL=C; export LC_ALL    # standard locale ("C")
    fi
fi

if [ -f ${TRICK_HOME}/bin/gte ] && [ -f "${PERL}" ] ;  then

    if [ x$TRICK_DEBUG = x ] ; then
        TRICK_DEBUG=0 ; export TRICK_DEBUG
    fi

    unset TRICK_CC
    unset TRICK_HOST_CPU
    unset TRICK_HOST_TYPE

    # Clear TRICK_CFLAGS and TRICK_USER_LINK_LIBS
    TRICK_CFLAGS="" ; export TRICK_CFLAGS
    TRICK_USER_LINK_LIBS="" ; export TRICK_USER_LINK_LIBS

    #################################################################
    # Get variables from gte
    #################################################################
    TRICK_HOST_CPU="`${TRICK_HOME}/bin/gte TRICK_HOST_CPU`"
    export TRICK_HOST_CPU
    TRICK_HOST_TYPE="`${TRICK_HOME}/bin/gte TRICK_HOST_TYPE`"
    export TRICK_HOST_TYPE
    TRICK_CFLAGS="`${TRICK_HOME}/bin/gte TRICK_CFLAGS`"
    export TRICK_CFLAGS
    XML_CATALOG_FILES="`${TRICK_HOME}/bin/gte XML_CATALOG_FILES`"
    export XML_CATALOG_FILES

    #################################################################
    # Append Trick directories to system environment
    #################################################################

    if [ $MANPATH ] ; then
        MANPATH=`"${PERL}" -e \
                 'print join":",grep{! $s{$_}++ }split/:/,\
                 "$ENV{MANPATH}:$ENV{TRICK_HOME}/man"'`
    else
       if [ "${TRICK_HOST_TYPE}" = "IRIX_6.x" ] ; then
            MANPATH="/usr/share/catman:/usr/share/man:/usr/catman:/usr/man:${TRICK_HOME}/man"
            export MANPATH
       elif [ "${TRICK_HOST_TYPE}" = "SunOS_5.6_plus" ] ; then
            MANPATH="/usr/share/man:${TRICK_HOME}/man"
            export MANPATH
       fi
    fi

    #################################################################
    # Cut out duplicate entries in env vars
    #################################################################
    PATH=`"${PERL}" -e \
          'print join":",grep{! $s{$_}++}split/:/,\
          "$ENV{PATH}:$ENV{TRICK_HOME}/bin_$ENV{TRICK_HOST_CPU}:$ENV{TRICK_HOME}/bin"'`

    # This section is for IRIX and SunOS that normally do not
    # have an up-to-date perl version.  Use the one we compile.
    # If you do have an up-to-date version (5.6 or higher) this section is not required
    if [ "${TRICK_HOST_TYPE}" = "IRIX_6.x" -o "${TRICK_HOST_TYPE}" = "SunOS_5.6_plus" ] ; then
        trick_perl=${TRICK_HOME}/bin_${TRICK_HOST_CPU}/perl
        trick_bin=${TRICK_HOME}/bin
        alias CP='            $trick_perl $trick_bin/CP'
        alias ICG='           $trick_perl $trick_bin/ICG'
        alias MIS='           $trick_perl $trick_bin/MIS'
        alias catalog='       $trick_perl $trick_bin/catalog'
        alias depend_cp='     $trick_perl $trick_bin/depend_cp'
        alias elwc='          $trick_perl $trick_bin/elwc'
        alias ftn_depend='    $trick_perl $trick_bin/ftn_depend'
        alias gte='           $trick_perl $trick_bin/gte'
        alias icg_dep='       $trick_perl $trick_bin/icg_dep'
        alias mis_dep='       $trick_perl $trick_bin/mis_dep'
        alias trick_version='$trick_perl $trick_bin/trick_version'
        if [ x$TRICK_MAKE = x ] ; then
                alias make='${TRICK_HOME}/bin_${TRICK_HOST_CPU}/make'
        fi
    fi

    alias tdt='TRICK_DEBUG=`expr \( ${TRICK_DEBUG} + 1 \) % 5`; TRICK_CFLAGS=`${TRICK_HOME}/bin/gte TRICK_CFLAGS`; echo "TRICK_CFLAGS=`printenv TRICK_CFLAGS`"'

fi


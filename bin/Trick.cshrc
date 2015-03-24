# Trick.cshrc
# This provides the default Trick environment for Trick users.
# Users may override any variable within their .Trick_user_cshrc

#################################################################
# Is Trick installed where TRICK_HOME is?
#################################################################
set PERL = `which perl`
if ( ! -e ${TRICK_HOME}/bin/gte ) then
    echo "${TRICK_HOME}/bin/gte doesn't exist... exiting .Trick_cshrc\!\!"
else if ( ! -e "${PERL}" ) then
    if ( -e /usr/bin/perl ) then
        set PERL = /usr/bin/perl
    else
        echo "${PERL} Perl doesn't exist... Exiting .Trick_cshrc\!\!"
    endif
endif

# Fix locale settings to avoid perl problems on MacOS
set OS_NAME=`uname -s`
if ( $OS_NAME == "Darwin" ) then
    # if the LC_ALL environment variable doesn't exist
    if ( ! $?LC_ALL ) then
        # Prevent this error-> "perl: warning: Setting locale failed."
        setenv LC_ALL C    # standard locale ("C")
    endif
endif

if (-e ${TRICK_HOME}/bin/gte && -e "${PERL}") then

    if ( ! $?TRICK_DEBUG ) then
        setenv TRICK_DEBUG 0
    endif

    unsetenv TRICK_CC
    unsetenv TRICK_HOST_CPU
    unsetenv TRICK_HOST_TYPE

    # Clear TRICK_CFLAGS and TRICK_USER_LINK_LIBS
    setenv TRICK_CFLAGS
    setenv TRICK_USER_LINK_LIBS

    #################################################################
    # Get variables from gte
    #################################################################
    setenv TRICK_HOST_CPU "`${TRICK_HOME}/bin/gte TRICK_HOST_CPU`"
    setenv TRICK_HOST_TYPE "`${TRICK_HOME}/bin/gte TRICK_HOST_TYPE`"
    setenv TRICK_CFLAGS "`${TRICK_HOME}/bin/gte TRICK_CFLAGS`"
    setenv XML_CATALOG_FILES "`${TRICK_HOME}/bin/gte XML_CATALOG_FILES`"

    #################################################################
    # Append Trick directories to system environment
    #################################################################

    if ( $?MANPATH ) then
        setenv MANPATH `"${PERL}" -e 'print join":", grep{! $s{$_}++ }split/:/, "$ENV{MANPATH}:$ENV{TRICK_HOME}/man"'`
    else
        if ( "${TRICK_HOST_TYPE}" == "IRIX_6.x" ) then
        setenv MANPATH "/usr/share/catman:/usr/share/man"
        setenv MANPATH "${MANPATH}:/usr/catman"
        setenv MANPATH "${MANPATH}:/usr/man:${TRICK_HOME}/man"
        else if ( "${TRICK_HOST_TYPE}" == "SunOS_5.6_plus" ) then
            setenv MANPATH "/usr/share/man:${TRICK_HOME}/man"
        endif
    endif

    #################################################################
    # Cut out duplicate entries in env vars
    #################################################################
    setenv PATH "${PATH}:${TRICK_HOME}/bin_${TRICK_HOST_CPU}"
    setenv PATH "${PATH}:${TRICK_HOME}/bin"
    setenv PATH `"${PERL}" -e 'print join":", grep{! $s{$_}++ }split/:/,"$ENV{PATH}"'`

    # For the Trick tutorial and a default setting for GR_DATABASE_HOME
    setenv GR_DATABASE_HOME ${TRICK_HOME}/gr_database
    setenv FSL_DATA ${TRICK_HOME}/gr_database/fonts

    alias tdt 'setenv TRICK_DEBUG `expr \( ${TRICK_DEBUG} + 1 \) % 5` ; \
               setenv TRICK_CFLAGS "`${TRICK_HOME}/bin/gte TRICK_CFLAGS`";\
               echo "TRICK_CFLAGS=`printenv TRICK_CFLAGS`"'

endif

/* IDENTIFICATION:    Robert W. Bailey    LinCom Corporation
            Created: January 1989 */

/* PURPOSE: Localize all unix system user commands used in 'C' routines */

#ifndef _UNIX_COMMANDS
#define _UNIX_COMMANDS

#if __sgi
#define unix_ssh "/usr/freeware/bin/ssh"
#define unix_rsh "/usr/bsd/rsh"
#elif __sun
#define unix_ssh "/usr/local/bin/ssh"
#define unix_rsh "/usr/bin/rsh"
#else
#define unix_ssh "/usr/bin/ssh"
#define unix_rsh "/usr/bin/rsh"
#endif

#endif


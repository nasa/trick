/*
   PURPOSE: (Floating point exception handler)

   PROGRAMMERS: (((Robert W. Bailey) (LinCom) (April 1992) (--) (Realtime)))
 */

#include <signal.h>
#include <stdio.h>
#include <string.h>

#include "trick/Executive.hh"
#include "trick/exec_proto.hh"

/**
 * @relates Trick::Executive
 * C binded function to handle the SIGFPE signal.  The handler prints which type of
 * floating point error has occured and calls exec_terminate()
 * @return void
 */
void fpe_sig_handler(int sig __attribute__((unused)), siginfo_t * sip, void * uap __attribute__((unused)))
{
    Trick::Executive * E = exec_get_exec_cpp();
    E->fpe_handler(sip);
}

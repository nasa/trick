/*
   PURPOSE: (This function switches on signal type to act on specific signal. This function performs the signal
   handling for both master and child processes.)

   REFERENCE: (((Bailey, R.W, and Paddock, E.J.) (Trick Simulation Environment) (NASA:JSC #37943) (JSC / Engineering
   Directorate / Automation and Robotics Division) (June 1994) (--)))

   ASSUMPTIONS AND LIMITATIONS: ((None))

   CLASS: (N/A)

   LIBRARY DEPENDENCY: ((none))

   PROGRAMMERS: (((Eddie J. Paddock) (MDSSC) (April 1992) (--) (Realtime)) ((Scott Killingsworth) (LinCom) (September
   1997) (--) (Issue #1016))) */

#include <stdlib.h>
#include <signal.h>
#include <setjmp.h>
#include <unistd.h>

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

#include "trick/Executive.hh"

/**
@design
-# This signal handler processes SIGBUS and SIGSEGV
-# Write a message to stderr that a signal has been caught.
-# If attach_debugger == true, attempt to attach a debugger to the current process.
-# Else if stack_trace == true, attempt to attach a debugger to print a stack trace.
-# Exit the process with a -2 return.
*/

void Trick::Executive::signal_handler(int sig) {

    // print a message to stderr. Use the write function because it is async signal safe.
    write( 2 , "\033[31mProcess terminated by signal " , 34 ) ;
    switch (sig) {
        case SIGBUS:                  /* Bus error */
            write( 2 , "SIGBUS" , 6 ) ;
            break;
        case SIGSEGV:                 /* Segmentation fault */
            write( 2 , "SIGSEGV" , 7 ) ;
            break;
        case SIGABRT:                 /* Abort */
            write( 2 , "SIGABRT" , 7 ) ;
            break;
        default:                      /* Unrecognized signal */
            write( 2 , "unknown" , 7 ) ;
            break;
    }
    write( 2 , "\033[0m\n" , 5 ) ;

    /**********************************************************************
     * Attempt to attach with debugger or print stack trace.  Not a requirement.
     * sprintf and system are not async signal safe, but we don't have anything to lose.
     */
    if ( ! debugger_command.empty() ) {
#if __linux
        char command[1024];
        if (attach_debugger == true) {
            sprintf(command, "%s -silent /proc/%d/exe %d", debugger_command.c_str(), getpid(), getpid());
            system(command);
        } else if (stack_trace == true ) {
            sprintf(command, "%s -silent -batch -x ${TRICK_HOME}/share/trick/gdb_commands "
                    "/proc/%d/exe %d", debugger_command.c_str(), getpid(), getpid());
            system(command);
        }
#elif __APPLE__
        char command[2048];
        char path[1024] ;
        uint32_t size = sizeof(path) ;
        if (_NSGetExecutablePath(path, &size) == 0 ) {
            if (attach_debugger == true) {
                write( 2 , "Attempting to attach debugger... standby.\n" , 41 ) ;
                sprintf(command, "%s -silent %s %d", debugger_command.c_str(), path, getpid());
                system(command);
            } else if (stack_trace == true ) {
                write( 2 , "Attempting to generate stack trace... standby.\n" , 47 ) ;
                sprintf(command, "%s -batch -s ${TRICK_HOME}/share/trick/lldb_commands -p %d",
                 debugger_command.c_str(), getpid());
                system(command);
            }
        }
#endif
    }

    // Sim state is unknown, don't return from handler.
    _exit(sig) ;
}

/**
@design
-# If enable_freeze == true toggle the mode between run/freeze. Design [@ref d_exec_signal_0]
-# Else if this the first time a <CTRL-C> is handled set the exec_command to ExitCmd.
-# If a graceful shutdown has been tried then write an exit message and exit.
*/
void Trick::Executive::ctrl_c_handler() {
    static bool graceful_shutdown_tried = false ;

    /* if enable_freeze == true toggle the mode between run/freeze. */
    if (enable_freeze == true) {
        if ( mode == Run) {
            exec_command = FreezeCmd ;
        } else {
            exec_command = RunCmd ;
        }
    } else if ( graceful_shutdown_tried == false ) {
        write( 2 , "\033[31mSetting exec_command = ExitCmd to shutdown simulation.\nIf process still unresponsive, type <CTRL-C> again for immediate exit.\033[0m\n" , 135 ) ;
        exec_command = ExitCmd ;
        graceful_shutdown_tried = true ;
    } else {
        write( 2 , "\033[31mProcess terminated by signal SIGINT <CTRL-C>\033[0m\n" , 54 ) ;
        _exit(SIGINT) ;
    }
}

/**
@design
-# Write an exit message
-# Exit the simulation immediately
*/
void Trick::Executive::term_handler() {
    write( 2 , "\033[31mProcess terminated by signal SIGTERM\033[0m\n" , 46 ) ;
    _exit(SIGTERM) ;
}

/**
@design
-# Write an exit message
-# Exit the simulation immediately
*/
void Trick::Executive::usr1_handler() {
    write( 2 , "\033[31mProcess terminated by call to exec_signal_terminate()\033[0m\n" , 63 ) ;
    _exit(SIGUSR1) ;
}


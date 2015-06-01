/*
   (This source file contains two functions that set up and remove a health and status spooling function which uses an
   ipc msg queue for msg spooling from multiple processes. )

   REFERENCE: (((Paddock, E.J) (Trick Runtime Executive Manual) (NASA:JSC #......) (JSC / Engineering Directorate /
   Automation and Robotics Division) (April 1992) (--)) (See Also: ))


   ASSUMPTIONS AND LIMITATIONS: ((Msg queue allocations must be within IPC limits of UNIX Kernal))

   CLASS: (N/A)

   LIBRARY DEPENDENCY: ((none))

   PROGRAMMERS: (((Eddie J. Paddock) (MDSSC) (April 1992) (--) (Realtime))) */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#ifdef __WIN32__
#include <winsock.h>
#include <malloc.h>
#else
#include <unistd.h>
#endif

#include "trick/hs_msg.h"

#ifdef STAND_ALONE
int send_hs(FILE * fp, char *format, ...);
void exec_terminate(char *file, char *error);

int send_hs(                    /* RETURN: -- -1 on failure , 0 for completion */
               FILE * fp,       /* In: File pointer, (usually stderr) */
               char *format,    /* In: Format of args */
               ...)
{                                      /* In: Variable length arg list */
    char *buf;
    va_list args;
    HS_MSG hs_msg;
    time_t date;

    va_start(args, format);

    (void) gethostname(hs_msg.hostname, (size_t) 48);

    /* Get the current date and time */
    date = time(NULL);
    strncpy(hs_msg.datebuff, asctime(localtime(&date)), (size_t) 27);

    /* Terminate the returned buffer which is always 24 chars long */
    hs_msg.datebuff[24] = '\0';

    hs_msg.ret = 0.0;
    hs_msg.parent_child_num = 0;
    (void) strcpy(hs_msg.sim_name, " ");

    /* Copy the message to print out */
    (void) vsprintf(hs_msg.msg, format, args);

    /* Rebuild message, adding id tag */
    buf = (char *) malloc(strlen(hs_msg.msg) + 128);
    sprintf(buf, "|%s|%s|%d|%.2f|%s|   %s\n",
            hs_msg.sim_name, hs_msg.hostname, hs_msg.parent_child_num, hs_msg.ret, hs_msg.datebuff, hs_msg.msg);
    strcpy(hs_msg.msg, buf);
    hs_msg.msgsize = strlen(hs_msg.msg);

    fprintf(fp, "%s\n", hs_msg.msg);

    free(buf);

    va_end(args);

    return (0);
}

void exec_terminate(char *file, /* In: Path name to file in which error occurred. */
                    char *error)
{                                      /* In: Error message */
    char outbuff[4096];

    sprintf(outbuff, "\n" "TERMINATED IN\n" "      ROUTINE:  %s\n" "   DIAGNOSTIC:\n" "%s\n" "\n", file, error);

    send_hs(stderr, "%s", outbuff);

    exit(0);
}
#endif

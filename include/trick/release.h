/*
PURPOSE:
    (This header file defines the "RELEASE()" macro for supported platforms.
     The release function should yield the processor from the processes that makes the call. This allows the UNIX schedular to arbitrate process
     priorities during spinloops based on the status of processes waiting
     to run)
PROGRAMMER:
    (((Eddie J. Paddock ) (LinCom) (2/96) (--) (Realtime))
     ((Scott Killingsworth ) (LinCom) (October 1997) (OSF1 port) (Issue #1233)))
*/

#ifndef RELEASE_H
#define RELEASE_H

#if __sgi

#define RELEASE() sginap(0)
#define RELEASE_1() sginap(1)

#elif __vxworks

#define RELEASE() taskDelay(1)
#define RELEASE_1() taskDelay(1)

#else

#include <time.h>

#define RELEASE() { struct timespec timeout;\
                    timeout.tv_sec = 0;\
                    timeout.tv_nsec = 0;\
                    nanosleep(&timeout, NULL);}

#define RELEASE_1() RELEASE()
#endif
#endif

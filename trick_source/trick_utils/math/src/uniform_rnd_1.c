/*
   PURPOSE: (ICDS uniform random number generator.  Almost a copy of ICDS' random number generator.  See code comments
   for any differences.)

   REFERENCE: ((ICDS))

   ASSUMPTIONS AND LIMITATIONS: ((If the user wants repeatability, a non zero seed should be used. If the user wants a
   random seed, a zero seed should be used.))

   CLASS: (scheduled)

   LIBRARY DEPENDENCY: ((uniform_rnd_1.o))

   PROGRAMMERS: (((Ray Li) (LMES/DUAL) (96 Nov) (1.0) (ICDS random number generator)) ((Robert Gay) (LinCom) (May 1998)
   (--) (Trick format)) ((David Hammen) (Titan-LinCom) (April 2001) (Renamed ICDS_Get_CPU_Time to total_CPU_time and
   made it local)) ((Robert Gay) (Titan-LinCom) (Aug 2001) (--) (Updates to put routines into trick math))) */

#include <math.h>
#if __Lynx__
#include <time.h>
#include <param.h>
#elif ( __ghs )
#include <sys/time.h>
#else
#include <sys/time.h>
#include <sys/param.h>
#endif
#include <sys/times.h>
#include <limits.h>
#include <unistd.h>
#include "trick/rand_generator.h"
#include "trick/trick_math.h"

#ifndef HZ
#define HZ 100                         /* This is a kludge to accomodate for Darwin. On Darwin HZ is actually a kernal
                                          global variable. I suppose I should use that but this will suffice. */
#endif

static double total_CPU_time(void);

double uniform_rnd_1(RAND_GENERATOR * G)
{                                      /* Inout: random number seed */
    static long a = 46341;      /* Value of a based on F. Clark note, 4/22/92 */
    static long m = 2147483647; /* Value of m based on F. Clark note, 4/22/92 */
    double new_random_number;

    /*
     *  NOTE:  This function behaves slightly different from
     *  ICDS's g_rnd().
     *   ICDS g_rnd():
     *     first-pass: build random number off input seed
     *     afterwards: modify input seed, then build random number
     *
     *   this function:
     *     build random number, then modify input seed.
     *
     *   This modification was made because the first-pass
     *   special case in g_rnd() means that the first model
     *   that uses g_rnd() will get the "first-pass" behavior on
     *   the first pass, while all other models that use g_rnd()
     *   will get the "afterwards" behavior on the first pass.
     *
     */

    if (!(G->seed_1)) {
        G->seed_1 = (unsigned long) total_CPU_time();
    }

    new_random_number = (double) G->seed_1 / (double) m;

    G->seed_1 = ((a * G->seed_1) % m);

    return (new_random_number);
}


/* This function returns the number of seconds that this process (and its wait'ed for children) has spend in execution
   (both user and system time) */
static double total_CPU_time(void)
{
    struct tms time_buffer;

    times(&time_buffer);

    return ((time_buffer.tms_utime + time_buffer.tms_cutime + time_buffer.tms_stime + time_buffer.tms_cstime) / HZ);

}

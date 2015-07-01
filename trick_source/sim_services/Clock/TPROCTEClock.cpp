/*
PURPOSE:
    ( gettimeofday clock )
PROGRAMMERS:
    (((Alex Lin) (NASA) (Feb 2012) (--) (--)))
*/

#include <stdio.h>

#include "trick/TPROCTEClock.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/release.h"

/**
@details
-# Calls the base Clock constructor
-# Sets the default device name
*/
Trick::TPROCTEClock::TPROCTEClock() : Clock(1000000, "TPROCTE") , dev_name("/dev/tpropci0") { }

/**
@details
-# This function is empty
*/
Trick::TPROCTEClock::~TPROCTEClock() { }

/**
@details
-# Opens the TPRO device
-# Returns error if unable to open device
-# Waits to be first user.
*/
int Trick::TPROCTEClock::clock_init() {
#ifdef _TPRO_CTE
    unsigned char  rv;

    /* Open the TPRO/TSAT device */
    rv = TPRO_open(&pBoard, (char *)dev_name.c_str());

    /* If unable to open the TPRO/TSAT device... */
    if (rv != TPRO_SUCCESS) {
        printf (" Could Not Open '%s'!! [%d]\n", dev_name.c_str(), rv);
        return (1);
    }

    /* Wait until this handle is the first user of the device. */
    if (TPRO_setPropDelayCorr(pBoard, NULL) != TPRO_SUCCESS) {
        printf(" Waiting to become first user...\n");
        while (TPRO_setPropDelayCorr(pBoard, NULL) != TPRO_SUCCESS);
    }

    set_global_clock() ;

    return 0 ;
#else
    message_publish(MSG_ERROR, "TPRO CTE card was not enabled when Trick was compiled.\n\
Please run ${TRICK_HOME}/configure --with-tpro=<directory holding CTE>, and recompile Trick\n");
    return -1 ;
#endif
}

/**
@details
-# Gets the TPRO time.
-# If sucessful convert the TPRO time to tics
*/
long long Trick::TPROCTEClock::wall_clock_time() {
#ifdef _TPRO_CTE
    unsigned char  rv;
    TPRO_TimeObj   tproTime;
    long long curr_time ;

    /* Get the TPRO/TSAT current time */
    rv = TPRO_getTime(pBoard, &tproTime);

    /* If unable to get the TPRO/TSAT current time... */
    if (rv != TPRO_SUCCESS) {
        printf (" Could not retrieve time from '%s'!! [%d]\n", dev_name.c_str(), rv);
        return 0 ;
    } else {
        curr_time = (long long)(tproTime.days * 86400LL * 1000000LL) ;
        curr_time += (long long)(tproTime.hours * 3600LL * 1000000LL) ;
        curr_time += (long long)(tproTime.minutes * 60LL * 1000000LL) ;
        curr_time += (long long)(tproTime.seconds * 1000000LL) ;
        curr_time += (long long)((tproTime.secsDouble - tproTime.seconds) * 1000000LL) ;
    }

    return (curr_time);
#else
    message_publish(MSG_ERROR, "TPRO CTE card was not enabled at compile time\n");
    return 0 ;
#endif
}

/**
@details
-# Get the current real time.
-# While the current real time is less than the requested time
   -# Release the processor momentarily
   -# Get the current real time.
*/
long long Trick::TPROCTEClock::clock_spin(long long req_time) {
#ifdef _TPRO_CTE
    Trick::Clock::clock_spin(req_time) ;
#else
    message_publish(MSG_ERROR, "TPRO CTE card was not enabled at compile time\n");
    return req_time ;
#endif

}

/**
@details
-# This function is empty
*/
int Trick::TPROCTEClock::clock_stop() {
#ifdef _TPRO_CTE
    unsigned char rv ;
    rv = TPRO_close(pBoard);
    /* If unable to close the TPRO/TSAT device... */
    if (rv != TPRO_SUCCESS) {
        printf (" Could Not Close Board!! [%d]\n", rv);
    }
#endif
    return 0 ;
}


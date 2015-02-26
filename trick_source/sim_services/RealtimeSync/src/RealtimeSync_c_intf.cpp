/*
 * $Id: RealtimeSync_c_intf.cpp 2640 2012-09-28 15:39:38Z alin $
 */

#include <stdio.h>
#include "sim_services/RealtimeSync/include/RealtimeSync.hh"
#include "sim_services/RealtimeSync/include/realtimesync_proto.h"
#include "sim_services/Executive/include/exec_proto.h"

/* Global singleton pointer to the real-time synchronization */
extern Trick::RealtimeSync * the_rts ;

/*************************************************************************/
/* These routines are the "C" interface to the real-time synchronization */
/*************************************************************************/

/**
 * @relates Trick::RealtimeSync
 * @copydoc Trick::RealtimeSync::enable
 * C wrapper for Trick::RealtimeSync::enable
 */
extern "C" int real_time_enable() {
    if ( the_rts != NULL ) {
        return the_rts->enable() ;
    }
    return(0) ;
}

/**
 * @relates Trick::RealtimeSync
 * @copydoc Trick::RealtimeSync::disable
 * C wrapper for Trick::RealtimeSync::disable
 */
extern "C" int real_time_disable() {
    if ( the_rts != NULL ) {
        return the_rts->disable() ;
    }
    return(0) ;
}

/**
 * @relates Trick::RealtimeSync
 * @copydoc Trick::RealtimeSync::start_realtime
 * C wrapper for Trick::RealtimeSync::start_realtime
 */
extern "C" int real_time_restart(long long ref_time ) {
    if ( the_rts != NULL ) {
        return the_rts->start_realtime(exec_get_software_frame() , ref_time) ;
    }
    return(0) ;
}

/**
 * @relates Trick::RealtimeSync
   @userdesc Return whether real-time synchronization is currently active.
   @par Python Usage:
   @code <my_int> = trick.is_real_time() @endcode
   @return Trick::RealtimeSync::active (integer converted from boolean)
 */
extern "C" int is_real_time() {
    if ( the_rts != NULL ) {
        return((int)(the_rts->active)) ;
    }
    return(0) ;
}

int real_time_change_clock(Trick::Clock * in_clock ) {
    if ( the_rts != NULL ) {
        return the_rts->change_clock(in_clock) ;
    }
    return -1 ;
}

const char * real_time_clock_get_name() {
    if ( the_rts != NULL ) {
        return the_rts->clock_get_name() ;
    }
    return NULL ;
}

int real_time_change_timer(Trick::Timer * in_sleep_timer ) {
    if ( the_rts != NULL ) {
        return the_rts->change_timer(in_sleep_timer) ;
    }
    return -1 ;
}

extern "C" int real_time_set_rt_clock_ratio(double in_clock_ratio) {
    return the_rts->set_rt_clock_ratio(in_clock_ratio) ;
}

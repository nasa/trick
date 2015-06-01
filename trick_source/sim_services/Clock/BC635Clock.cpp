/*
PURPOSE:
    ( gettimeofday clock )
PROGRAMMERS:
    (((Alex Lin) (NASA) (Feb 2012) (--) (--)))
*/

#include <stdio.h>

#include "trick/BC635Clock.hh"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "trick/release.h"

/**
@details
-# Calls the base Clock constructor
-# Sets the default device name
*/
Trick::BC635Clock::BC635Clock() : Clock(1000000, "BC635") {
#ifdef _BC635
    mode = MODE_FREE ;
#endif
}

/**
@details
-# This function is empty
*/
Trick::BC635Clock::~BC635Clock() { }

void Trick::BC635Clock::set_mode(unsigned int in_mode) {
    mode = in_mode ;
}

unsigned int Trick::BC635Clock::get_mode() {
    return mode ;
}

/**
@details
-# Opens the BC635 device
-# Returns error if unable to open device
-# Waits to be first user.
*/
int Trick::BC635Clock::clock_init() {
#ifdef _BC635
    h_bc635 = bcStartPci();

    if ( !h_bc635 ) {
        message_publish( MSG_ERROR, "%s:%s:%d: Error on bcStartPci. Can't open Symmetricom BC635 PCI card.",
                __FILE__, __FUNCTION__, __LINE__ );
        return 1 ;
    }

    /* We have to use binary time in order to get sub-second time data */
    if ( bcSetTimeFormat( h_bc635, FORMAT_BINARY ) == FALSE ) {
        message_publish( MSG_ERROR, "bcSetTimeFormat failed" );
        return -1 ;
    }

    /* Set the clock source to internal.. we're not using an external oscillator */
    if ( bcSetClkSrc( h_bc635, CLK_INT) == FALSE ) {
        message_publish( MSG_ERROR, "bcSetClkSrc failed to set the clock source to internal" );
        return -1 ;
    }

    /* Sync the RTC to External Time Data */
    if ( bcSyncRtc( h_bc635 ) == FALSE ) {
        message_publish( MSG_ERROR, "bcSyncRtc failed to set the RTC to External Time Data" );
        return -1 ;
    }

    /* Set the mode, e.g. to read input time codes, or freewheel */
    /* (Note: This function returns void, so can't do error checking) */
    bcSetMode( h_bc635, mode );

    // Set local time offset to 0 (so we use GMT)
    bcSetLocalOffsetFlag( h_bc635, LOCAL_OFF_DISABLE );

    if ( bcSetGenCode( h_bc635, TCODE_IRIG_B ) == FALSE ) {
        message_publish( MSG_ERROR, "bcSetGenCode failed to set output format to IRIG-B" );
        return -1 ;
    }

    /* IRIG B002, B122 (BCD) */
    if ( bcSetGenCodeEx( h_bc635, TCODE_IRIG_B, TCODE_IRIG_SUBTYPE_2 ) == FALSE ) {
        message_publish( MSG_ERROR, "bcSetGenCodeEx failed to set output format to IRIG-B" );
        return -1 ;
    }

    /* Set output frequency to 10Mhz */
    DWORD ctlreg;
    if ( bcReadReg( h_bc635, PCI_CONTROL_OFFSET, &ctlreg ) == FALSE ) {
        message_publish( MSG_ERROR, "bcReadReg failed" );
        return -1 ;
    }
    ctlreg &= 0x3F;
    if ( bcWriteReg( h_bc635, PCI_CONTROL_OFFSET, ctlreg ) == FALSE ) {
        message_publish( MSG_ERROR, "bcWriteReg failed" );
        return -1 ;
    }

    set_global_clock() ;

    return 0 ;
#else
    message_publish(MSG_ERROR, "BC635 card was not enabled when Trick was compiled.\n\
Please run ${TRICK_HOME}/configure --bc635=<directory holding CTE>, and recompile Trick\n");
    return -1 ;
#endif
}

/**
@details
-# Gets the BC635 time.
-# If sucessful convert the BC635 time to tics
*/
long long Trick::BC635Clock::wall_clock_time() {
#ifdef _BC635
    BOOL ret ;
    DWORD ulpMaj, ulpMin ;
    WORD ulpNano;
    BYTE stat ;
    long long curr_time ;

    ret = bcReadBinTimeEx( h_bc635, &ulpMaj, &ulpMin, &ulpNano, &stat) ;

    if ( ret == FALSE ) {
        message_publish(MSG_ERROR, "Could not retrieve time from BC635\n") ;
    } else {
        // I believe that ulpMaj = num seconds and ulpMin is num microseconds. How convenient.
        curr_time = (long long)ulpMaj * 1000000LL + ulpMin ;
    }

    return (curr_time);
#else
    message_publish(MSG_ERROR, "BC635 card was not enabled at compile time\n");
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
long long Trick::BC635Clock::clock_spin(long long req_time) {
#ifdef _BC635
    Trick::Clock::clock_spin(req_time) ;
#else
    message_publish(MSG_ERROR, "BC635 card was not enabled at compile time\n");
    return req_time ;
#endif

}

/**
@details
-# This function is empty
*/
int Trick::BC635Clock::clock_stop() {
#ifdef _BC635
    // Don't do this because exec_terminate reads the time
    //bcStopPci( h_bc635 );
#endif
    return 0 ;
}


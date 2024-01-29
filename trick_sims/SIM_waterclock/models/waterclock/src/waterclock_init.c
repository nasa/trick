/******************************* TRICK HEADER ****************************
PURPOSE: (Set the initial data values)
*************************************************************************/

/* Model Include files */
#include <math.h>
#include "../include/waterclock.h"

/* default data job */
int waterclock_default_data( WATERCLOCK* WC ) {

    WC->time							= 0.0 ;

    WC->input_flow             		 	= 0.5;
    WC->intake_clock_spout_flowrate     = 0.0;
    WC->intake_overflow_flowrate    	= 0.0;

    WC->intake_bucket_depth         	= 1.0;
    WC->intake_bucket_diam          	= 5.0;

    WC->intake_overflow_height      	= 0.8 * WC->intake_bucket_depth;
    WC->intake_overflow_diameter    	= 1.0;

    WC->intake_clock_spout_height       = 0.1;
    WC->intake_clock_spout_diameter     = 0.05;

    WC->timer_bucket_depth      		= 1.2;
    WC->timer_bucket_diam       		= 0.5;

    WC->intake_bucket_vol           	= 0.0;
    WC->timer_bucket_vol        		= 0.0;

    WC->total_ticks            	 		= 5;
    WC->current_tick            		= 0;

    WC->gravity                 		= 9.81;
    WC->pi                      		= 3.14;

    return 0 ;
}

/* initialization job */
int waterclock_init( WATERCLOCK* WC) {

    WC->tick_gap = WC->timer_bucket_depth / WC->total_ticks ;

    return 0 ;
}

/*********************************************************************
  PURPOSE: ( Trick numeric )
*********************************************************************/
#include <stddef.h>
#include <stdio.h>
#include <math.h>
#include "trick/integrator_c_intf.h"
#include "../include/waterclock_numeric.h"
#include "trick/exec_proto.h"
#include "trick/message_proto.h"
#include <math.h>

int waterclock_deriv(WATERCLOCK* WC) {

    //Make sure water level is up to date
    waterclock_update_water_level(WC);

    //Clock spout area
    double spout_radius = WC->intake_clock_spout_diameter / 2;
    double spout_area = M_PI * spout_radius * spout_radius;

    //Overflow spout area
    double overflow_radius = WC->intake_overflow_diameter / 2;
    double overflow_area = M_PI * overflow_radius * overflow_radius;

    //Calculate flow rate using Torricelli's equation ( V = sqrt(2gh) ) to find water velocity. Multiple velocity by spout area to find flow rate.
    //Calculate input bucket spout flow rate.
    if(WC->intake_water_level > WC->intake_clock_spout_height)
        WC->intake_clock_spout_flowrate = spout_area * sqrt(2 * WC->gravity * (WC->intake_water_level - WC->intake_clock_spout_height) );
    else
        WC->intake_clock_spout_flowrate = 0.0;

    //Calculate input bucket over flow rate
    if(WC->intake_water_level > WC->intake_overflow_height)
        WC->intake_overflow_flowrate = overflow_area * sqrt(2 * WC->gravity * (WC->intake_water_level - WC->intake_overflow_height) );
    else
        WC->intake_overflow_flowrate = 0.0;

    //Need to know net flow of the source, overflow spout, and clock spout for integration
    WC->intake_bucket_net_flow = WC->input_flow - WC->intake_clock_spout_flowrate - WC->intake_overflow_flowrate;

    return(0);
}

int waterclock_integ(WATERCLOCK* WC) {
    int ipass;

    load_state(
        &WC->intake_bucket_vol,
        &WC->timer_bucket_vol,
        NULL);

    load_deriv(
        &WC->intake_bucket_net_flow,
        &WC->intake_clock_spout_flowrate,
        NULL);

    ipass = integrate();

    unload_state(
        &WC->intake_bucket_vol,
        &WC->timer_bucket_vol,
        NULL );

    waterclock_update_water_level(WC);

    return(ipass);
}

//Since we are inetgrating over water volume, we must convert volume to water level in order to track how quickly the timer bucket is filling up.
void waterclock_update_water_level(WATERCLOCK* WC)
{
    //Calculate Input bucket water level
    double intake_bucket_radius = WC->intake_bucket_diam / 2;
    double intake_bucket_base = M_PI * intake_bucket_radius * intake_bucket_radius;
    WC->intake_water_level = WC->intake_bucket_vol / intake_bucket_base;

    //Calculate Timer bucket water level
    double timer_bucket_radius = WC->timer_bucket_diam / 2;
    double timer_bucket_base = M_PI * timer_bucket_radius * timer_bucket_radius;
    WC->timer_water_level = WC->timer_bucket_vol / timer_bucket_base;

}

//When timer bucket fills, drain it instantly (we have a magic siphon)
double waterclock_overflow_timer( WATERCLOCK* WC ) {
    double tgo;
    double now;

    WC->rf1.error =  WC->timer_bucket_depth - WC->timer_water_level;
    now = get_integ_time();
    tgo = regula_falsi( now, &(WC->rf1) );
    if (tgo == 0.0) {
        now = get_integ_time() ;
        reset_regula_falsi( now, &(WC->rf1) ) ;
        WC->timer_water_level = 0; //Instantly drains
        WC->timer_bucket_vol = 0; //Instantly drains
        WC->current_tick = 0;
        message_publish(MSG_NORMAL, "WATER CLOCK RESET\n" ) ;
    }
    return (tgo) ;
}

//Detect when the arrow has passed a new tick, print some diagnostics.
double waterclock_tick_change( WATERCLOCK* WC ) {
    double tgo;
    double now;

    WC->rf2.error =  WC->tick_gap - (WC->timer_water_level - ( WC->current_tick * WC->tick_gap ));
    now = get_integ_time();
    tgo = regula_falsi( now, &(WC->rf2) );
    if (tgo == 0.0) {
        now = get_integ_time() ;
        reset_regula_falsi( now, &(WC->rf2) ) ;
        if( (WC->current_tick < WC->total_ticks) && (WC->current_tick >= 0) )
        {
            WC->current_tick += 1;
            message_publish(MSG_NORMAL, "Tick %d, Sim Time %f, Water Level %f\n", WC->current_tick, exec_get_sim_time(), WC->timer_water_level) ;
        }
        else
            fprintf(stderr, "ERROR, SOMETHING WENT VERY WRONG!\n" ) ;
    }
    return (tgo) ;
}

/*************************************************************************
PURPOSE: (Represent the state and initial conditions of the water clock)
**************************************************************************/
#ifndef WATERCLOCK_H
#define WATERCLOCK_H

#include "trick/regula_falsi.h"

typedef struct {


    //////////////////////////

    /*    ASSUMPTIONS
    *         1) For the purpose of determining if a pipe is submerged, it will be treated as a single point at the center of the pipe
    *         2) When the timing bucket is filled, a siphon will drain it instaneously
    *         3) Buckets are cylindrical
    */
 
    /*    DESCRIPTION
    *         This is a simulation of a simple water clock. The clock consists of two cylinders (buckets).
    *         The first bucket is known as the intake bucket. It receives water from an unspecified external source (like a stream).
    *         The intake bucket has two cylindrical spouts extending from its sides, one known as an overflow spout and the other the clock spout.
    *         The overflow spout is simply for maintaing an equilibrium of the water level. It keeps the intake bucket from exceeding a certain water level, thus ensuring it's water level is consistent.
    *         The clock spout pours water into the second bucket (timer bucket). In an ideal clock, the water level of the intake bucket will remain consistent, meaning the clock spout will output at a constant rate and operate as an accurate clock.
    *         The timer bucket receives water from the clock spout. The timer bucket has no spouts, it fills until it is full, upon which a siphon instantaneously drains it.
    *         There exists a float in the timer bucket, connected to an arrow which will point to vertical tick marks. As the timer bucket fills, the arrow will move vertically along these ticks, indicating how much time has passed.
    *         Depending on the configuration of the clock (size of the buckets, size of the spouts, external water source rate, and the number/spacing of ticks) you can create a clock that tracks a variety of times.
    */

    double time;                            /* s Model time */

    double input_flow;                      /* m^3/s Flow rate of the external water source to the water clock */
    double intake_clock_spout_flowrate;     /* m^3/s Intake bucket output spout pipe flow rate */
    double intake_overflow_flowrate;        /* m^3/s Intake bucket overflow pipe flow rate */
    double intake_bucket_net_flow;          /* m^3/s Intake bucket net flow rate */

    double intake_bucket_depth;             /* m Intake bucket depth */
    double intake_bucket_diam;              /* m Intake bucket diameter */

    double intake_overflow_height;          /* m Intake bucket overflow pipe height of center */
    double intake_overflow_diameter;        /* m Intake bucket overflow pipe diameter */

    double intake_clock_spout_height;       /* m Intake bucket output spout pipe height of center */
    double intake_clock_spout_diameter;     /* m Intake bucket output spout pipe diameter */

    double timer_bucket_depth;              /* m Timer bucket depth */
    double timer_bucket_diam;               /* m Timer bucket diameter */

    double intake_bucket_vol;               /* m^3 Intake bucket water volume */
    double intake_water_level;              /* m Intake bucket water level */
    double timer_bucket_vol;                /* m^3 Timer bucket water volume */
    double timer_water_level;               /* m Timer bucket water level */

    double tick_gap;                        /* m Distance between tick marks */
    int total_ticks;                        /* -- Total number of ticks on the timer */
    int current_tick;                       /* --  The current timer tick to have crossed the threshold*/

    double gravity;                         /* m/s^2 Gravity constant */
    double pi;                              /* -- PI constant */

    REGULA_FALSI rf1 ;
    REGULA_FALSI rf2 ;

} WATERCLOCK ;

#ifdef __cplusplus
extern "C" {
#endif
    int waterclock_default_data(WATERCLOCK*) ;
    int waterclock_init(WATERCLOCK*) ;
    int waterclock_shutdown(WATERCLOCK*) ;
#ifdef __cplusplus
}
#endif

#endif

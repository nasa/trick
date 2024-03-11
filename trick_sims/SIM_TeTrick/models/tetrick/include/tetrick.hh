/*************************************************************************
PURPOSE: (Represent the state and initial conditions of the water clock)
**************************************************************************/
#ifndef TETRICK_HH
#define TETRICK_HH

#include "trick/regula_falsi.h"

class TeTrick 
{
public:
	double time;                    		/* s Model time */

	double input_flow;              		/* m^3/s Flow rate of the external water source to the water clock */
	double intake_clock_spout_flowrate;       	/* m^3/s Intake bucket output spout pipe flow rate */
	double intake_overflow_flowrate;    		/* m^3/s Intake bucket overflow pipe flow rate */
	double intake_bucket_net_flow;      		/* m^3/s Intake bucket net flow rate */

	double intake_bucket_depth;        		/* m Intake bucket depth */
	double intake_bucket_diam;          		/* m Intake bucket diameter */

	double intake_overflow_height;      		/* m Intake bucket overflow pipe height of center */
	double intake_overflow_diameter;    		/* m Intake bucket overflow pipe diameter */

	double intake_clock_spout_height;       	/* m Intake bucket output spout pipe height of center */
	double intake_clock_spout_diameter;     	/* m Intake bucket output spout pipe diameter */

	double timer_bucket_depth;      		/* m Timer bucket depth */
	double timer_bucket_diam;       		/* m Timer bucket diameter */

	double intake_bucket_vol;       		/* m^3 Intake bucket water volume */
	double intake_water_level;      		/* m Intake bucket water level */
	double timer_bucket_vol;        		/* m^3 Timer bucket water volume */
	double timer_water_level;       		/* m Timer bucket water level */

	double tick_gap;                		/* m Distance between tick marks */
	int total_ticks;                		/* -- Total number of ticks on the timer */
	int current_tick;               		/* --  The current timer tick to have crossed the threshold*/

	double gravity;                 		/* m/s^2 Gravity constant */
	double pi;                      		/* -- PI constant */

	REGULA_FALSI rf1 ;
	REGULA_FALSI rf2 ;

private:
	static TeTrick* game;

public:
	int tetrick_default_data();
	int tetrick_init();
	int tetrick_shutdown();


};
#endif

/*************************************************************************
PURPOSE: (Simulate a model rocket.)
LIBRARY DEPENDENCIES:
    ((rocket/src/Rocket.o))
**************************************************************************/
#ifndef _rocket_hh_
#define _rocket_hh_

#include "Environment.hh"
#include "Motor.hh"

class Rocket {
public:
    Environment env;
    Motor motor;

    double vel_unit[2];
    double thrust_force[2];
    double drag_force[2];
    double gravity_force[2];
    double total_force[2];

    double mission_time ; /* s    time elapsed since launch */
    double mission_time_rate;
    double cross_sectional_area;
    double coefficient_of_drag;
    double empty_mass;
    double total_mass ;   /* kg   xyz-position */
    double mass_rate ;
    double position[2] ;       /* m    xyz-position */
    double velocity[2] ;       /* m/s  xyz-velocity */
    double acc[2] ;       /* m/s2 xyz-acceleration  */

    int default_data();
    int state_init();
    int state_deriv();
    int state_integ();

};
#endif

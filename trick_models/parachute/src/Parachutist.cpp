/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate a skydiver jumping from very high altitude.)
LIBRARY DEPENDENCY:
    ((Parachutist.o)(interpolate.o))
PROGRAMMERS:
    (((John M. Penn) (L3 Communications) (March 2015) (Trick Refresher Project)))
*******************************************************************************/
#include "../include/Parachutist.hh"
#include "../include/interpolate.h"
#include <iostream>

// The following three data arrays come from:
// http://www.engineeringtoolbox.com/standard-atmosphere-d_604.html
// U.S Standard Atmosphere Air Properties in SI Units
#define NUM_ELEMENTS 21

// Units = meters (above sea level).
const double altitude_array[NUM_ELEMENTS] = {
 -1000.0,     0.0,  1000.0,  2000.0,  3000.0,  4000.0,  5000.0,  6000.0,
  7000.0,  8000.0,  9000.0, 10000.0, 15000.0, 20000.0, 25000.0, 30000.0,
 40000.0, 50000.0, 60000.0, 70000.0, 80000.0 };

// Units = kilograms per cubic meter.
const double air_density_array[NUM_ELEMENTS] = {
    1.347, 1.225, 1.112, 1.007, 0.9093, 0.8194, 0.7364, 0.6601,
   0.5900, 0.5258, 0.4671, 0.4135, 0.1948, 0.08891, 0.04008, 0.01841,
 0.003996, 0.001027, 0.0003097, 0.00008283, 0.00001846 };

// Units = meters per second squared.
const double gravity_array[NUM_ELEMENTS] = {
9.810, 9.807, 9.804, 9.801, 9.797, 9.794, 9.791, 9.788,
9.785, 9.782, 9.779, 9.776, 9.761, 9.745, 9.730, 9.715,
9.684, 9.654, 9.624, 9.594, 9.564
};

const double Parachutist::crossSectionalArea_freefall  =  0.75 ;
const double Parachutist::crossSectionalArea_parachute = 30.00;
const double Parachutist::cooefficientOfDrag_freefall  =  0.75;
const double Parachutist::cooefficientOfDrag_parachute =  1.30;

int Parachutist::default_data() {

    position = 38969.3; //38969.3 meters = 127852 feet
    velocity = 0.0;
    acceleration = 0.0;
    parachuteDeploymentStartTime = 259; /* 4 minutes, 19 seconds*/
    parachuteDeploymentDuration  = 3;
    crossSectionalArea = crossSectionalArea_freefall;
    cooefficientOfDrag = cooefficientOfDrag_freefall;
    touchDown = false;
    mass = 82.0;

    return (0);
}

int Parachutist::state_init() {
    parachuteDeploymentEndTime = parachuteDeploymentStartTime + parachuteDeploymentDuration;
    return (0);
}

#include "sim_services/Executive/include/exec_proto.h"
int Parachutist::parachute_control() {

    double currentTime =  exec_get_sim_time();

    if ((currentTime >  parachuteDeploymentStartTime) &&
        (currentTime <= parachuteDeploymentEndTime)) {
        cooefficientOfDragRate = (cooefficientOfDrag_parachute - cooefficientOfDrag_freefall)
                / parachuteDeploymentDuration;
        crossSectionalAreaRate = (crossSectionalArea_parachute - crossSectionalArea_freefall)
                / parachuteDeploymentDuration;
    } else {
        cooefficientOfDragRate = 0.0;
        crossSectionalAreaRate = 0.0;
    }
    return (0);
}

int Parachutist::state_deriv() {

   // Calculate the force of gravity.
#if 1
   double g = 9.81;
#else
   double g = interpolate( position, altitude_array, gravity_array, NUM_ELEMENTS );
#endif

   double Force_gravity = mass * -g;

   // Calculate the force of drag.
   double air_density = interpolate( position, altitude_array, air_density_array, NUM_ELEMENTS );
   double Force_drag = cooefficientOfDrag * 0.5 * air_density * velocity * velocity * crossSectionalArea;

   // Sum the forces and calculate acceleration.
   double Force_total;

   // If skydiver has touched down then set state derivatives to zero.
   if ( touchDown ) {
       Force_total = 0.0;
       velocity = 0.0;
       acceleration = 0.0;
   } else {
       Force_total = Force_gravity + Force_drag ;
       acceleration = Force_total / mass;
   }

   /* RETURN: -- Always return zero. */
   return(0);
}

#include "sim_services/Integrator/include/integrator_c_intf.h"
int Parachutist::state_integ() {

   int integration_step;

   load_state( &position,
               &velocity,
               &cooefficientOfDrag,
               &crossSectionalArea,
               (double*)0
             );

   load_deriv ( &velocity,
                &acceleration,
                &cooefficientOfDragRate,
                &crossSectionalAreaRate,
                (double*)0
              );

   integration_step = integrate();

   unload_state( &position,
                 &velocity,
                 &cooefficientOfDrag,
                 &crossSectionalArea,
                 (double*)0
               );

   return(integration_step);
}

double Parachutist::touch_down() {
    double tgo ;
    double now ;

    /* error function: how far the skydiver is above ground */
    rf.error = position ;

    now = get_integ_time() ;
    tgo = regula_falsi( now, &rf ) ;
    if (tgo == 0.0) {
        now = get_integ_time() ;
        reset_regula_falsi( now, &rf);
        velocity = 0.0;
        acceleration = 0.0;
        touchDown = true;
        std::cout << "Touchdown: time = " << now << "." << std::endl;
    }
    return ( tgo ) ;
}

/********************************* TRICK HEADER *******************************
PURPOSE:
    (Target planet model parameter default initialization data.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC/Engineering Directorate/Automation, Robotics and Simulation Division)
      (March 1997)))
ASSUMPTIONS AND LIMITATIONS:
    ((Simple spherical Newtonian body.))
PROGRAMMERS:
   (((Edwin Z. Crues) (NASA JSC ER7) (Nov 2009) (--) (Target planet init.)))
*******************************************************************************/
#include "../include/target_body.h"

int target_earth_default_data(TargetBodyPlanet* target_planet) {
    /* Set the planetary gravitational constant. */
    target_planet->mu = 3.986012e5 * 1000000000;
    return(0);
}

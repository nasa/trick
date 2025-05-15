/*************************************************************************
PURPOSE: (Simulate a satellite orbiting the Earth.)
LIBRARY DEPENDENCIES:
    ((satellite/src/Satellite.o))
**************************************************************************/
#ifndef _satellite_hh_
#define _satellite_hh_

#define GRAVITATIONAL_CONSTANT 6.674e-11
#define EARTH_MASS 5.9723e24
#define EARTH_RADIUS 6367500.0

class Satellite {

    public:
        double pos[2] ;    /* m     xyz-position */
        double vel[2] ;    /* m/s   xyz-velocity */
        double acc[2] ;    /* m/s2  xyz-acceleration  */

        int default_data();
        int state_init();
        int state_deriv();
        int state_integ();
};
#endif




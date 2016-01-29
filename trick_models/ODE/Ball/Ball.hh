
/*
PURPOSE:
    (Bouncing ball)
LIBRARY DEPENDENCY:
    ((Ball.cpp))
*/

#ifndef ODEBALL_HH
#define ODEBALL_HH

#include <ode/common.h>
#include <ode/src/objects.h>

class Ball {
    public:

        dBodyID body;
        dGeomID geom;
        dMass m;

        Ball() ;
        int init() ;

} ;

#endif


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
#include "DrawStuff/DrawStuffObject.hh"

class Ball : public DrawStuffObject {
    public:

        dWorldID world;              // ** world
        dSpaceID space;              // ** space
        dJointGroupID contactgroup;  // ** contact

        dBodyID body;                // ** the ball body
        dGeomID geom;                // ** contact geometry
        dMass m;                     // ** the mass of the ball

        dReal * pos ;

        Ball() ;

        int init() ;
        int step(double time_step) ;
        int shutdown() ;

        virtual int draw() ;

        static void nearCallback(void *data, dGeomID o1, dGeomID o2) ;

} ;

#endif

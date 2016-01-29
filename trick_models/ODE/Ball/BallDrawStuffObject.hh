
/*
PURPOSE:
    (Bouncing ball)
LIBRARY DEPENDENCY:
    ((BallDrawStuffObject.cpp))
*/

#ifndef ODEBALLDRAWSTUFFOBJECT_HH
#define ODEBALLDRAWSTUFFOBJECT_HH

#include <ode/common.h>
#include "ODE/DrawStuff/DrawStuffObject.hh"

class BallDrawStuffObject : public DrawStuffObject {
    public:
        dGeomID geom ;

        BallDrawStuffObject() ;
        int init(dGeomID in_geom) ;
        virtual int draw() ;
} ;

#endif

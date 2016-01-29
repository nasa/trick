
#include <ode/ode.h>
#include "Ball.hh"
#include "ODE/OdeWorld/OdeWorld.hh"

Ball::Ball() {}

int Ball::init() {
    body = dBodyCreate (OdeWorld::getWorldID());
    geom = dCreateSphere (OdeWorld::getSpaceID(),0.5);
    dMassSetSphere (&m,1,0.5);
    dBodySetMass (body,&m);
    dGeomSetBody (geom,body);
    // set initial position
    dBodySetPosition (body,0,0,3);
    return 0 ;
}


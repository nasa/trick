
#include <ode/collision.h>
#include <drawstuff/drawstuff.h>
#include "BallDrawStuffObject.hh"

BallDrawStuffObject::BallDrawStuffObject() {}

int BallDrawStuffObject::init(dGeomID in_geom) {
    geom = in_geom ;
    add_object() ;
    return 0 ;
}

int BallDrawStuffObject::draw() {
    dsDrawSphere (dGeomGetPosition(geom),dGeomGetRotation(geom),dGeomSphereGetRadius(geom));
    return 0 ;
}


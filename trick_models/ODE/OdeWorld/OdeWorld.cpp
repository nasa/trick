
#include <ode/ode.h>
#include "OdeWorld.hh"

dWorldID OdeWorld::world ;
dSpaceID OdeWorld::space ;
dJointGroupID OdeWorld::contactgroup ;

extern int current_state ;

OdeWorld::OdeWorld() {}

int OdeWorld::init() {
    dInitODE ();
    world = dWorldCreate ();
    space = dHashSpaceCreate (0);
    dWorldSetGravity (world,0,0,-9.81);
    dWorldSetCFM (world,1e-5);
    dCreatePlane (space,0,0,1,0);
    contactgroup = dJointGroupCreate (0);
    return 0 ;
}

int OdeWorld::step( double time_step) {

    // find collisions and add contact joints
    dSpaceCollide (space,0,&OdeWorld::nearCallback);

    // step the simulation
    //dWorldQuickStep (world,time_step);
    dWorldStep (world,time_step);
    // remove all contact joints
    dJointGroupEmpty (contactgroup);

    return 0 ;
}

int OdeWorld::shutdown() {
    // clean up
    dJointGroupDestroy (contactgroup);
    dSpaceDestroy (space);
    dWorldDestroy (world);
    dCloseODE();

    return 0 ;
}

// this is called by dSpaceCollide when two objects in space are
// potentially colliding.
void OdeWorld::nearCallback (void *, dGeomID o1, dGeomID o2)
{
    dBodyID b1 = dGeomGetBody(o1);
    dBodyID b2 = dGeomGetBody(o2);
    dContact contact;
    contact.surface.mode = dContactBounce | dContactSoftCFM;
    // friction parameter
    contact.surface.mu = dInfinity;
    // bounce is the amount of "bouncyness".
    contact.surface.bounce = 0.999;
    // bounce_vel is the minimum incoming velocity to cause a bounce
    contact.surface.bounce_vel = 0.1;
    // constraint force mixing parameter
    contact.surface.soft_cfm = 0.001;
    if (int numc = dCollide (o1,o2,1,&contact.geom,sizeof(dContact))) {
        dJointID c = dJointCreateContact (OdeWorld::world,OdeWorld::contactgroup,&contact);
        dJointAttach (c,b1,b2);
    }
}

dWorldID OdeWorld::getWorldID() {
    return world ;
}

dSpaceID OdeWorld::getSpaceID() {
    return space ;
}



/*
PURPOSE:
    (Bouncing ball)
LIBRARY DEPENDENCY:
    ((Buggy.cpp))
*/

#ifndef BUGGY_HH
#define BUGGY_HH

#include <ode/common.h>
#include <ode/src/objects.h>
#include "DrawStuff/DrawStuffObject.hh"

#if SWIG
%apply float { dReal }
#endif

class Buggy : public DrawStuffObject {
    public:

        dWorldID world;              // ** world
        dSpaceID space;              // ** space
        dJointGroupID contactgroup;  // ** contact

        dReal speed ;
        dReal steer ;
        dReal * buggy_body_pos ;

        dReal gravity ;

        dReal chassis_length ;
        dReal chassis_width ;
        dReal chassis_height ;
        dReal chassis_mass ;

        dReal wheel_radius ;
        dReal wheel_width ;
        dReal wheel_mass ;

        dReal start_z ;

        Buggy() ;

        // Simulation jobs
        int init() ;
        int step(double time_step) ;
        int shutdown() ;

        // DrawStuff callbacks
        virtual int draw() ;
        virtual void command(int cmd) ;

        // collision callback
        static void nearCallback(void *data, dGeomID o1, dGeomID o2) ;

    protected:

        dBodyID body[4];             // ** buggy body and 3 wheels
        dJointID joint[3];           // ** joints from body to wheels

        dSpaceID car_space;          // ** car space. :P
        dGeomID box[1];              // ** collision for buggy body
        dGeomID sphere[3];           // ** collision for wheels

        dGeomID ground;              // ** ground
        dGeomID ground_box;          // ** ground

} ;

#endif

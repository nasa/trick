
/*
PURPOSE:
    (Create the ODE world)
LIBRARY DEPENDENCY:
    ((OdeWorld.cpp))
*/

#ifndef ODEWORLD_HH
#define ODEWORLD_HH

#include <ode/common.h>

class OdeWorld {
    public:
        static dWorldID world;
        static dJointGroupID contactgroup;
        static dSpaceID space;

        static dWorldID getWorldID() ;
        static dSpaceID getSpaceID() ;

        OdeWorld() ;

        int init() ;
        int step(double time_step) ;
        int shutdown() ;

        static void nearCallback(void *data, dGeomID o1, dGeomID o2) ;

} ;

#endif

/************************************************************************
PURPOSE: (Simulate a ... .)
LIBRARY DEPENDENCIES:
    ((CrewModule/src/CrewModuleDynamics.o)((CrewModule/src/CrewModuleShape.o)))
**************************************************************************/
#ifndef CREW_MODULE_DYNAMICS_HH
#define CREW_MODULE_DYNAMICS_HH

#include "../include/CrewModuleShape.hh"

class CrewModuleDynamics {
public:

    double R[3][3];                  /* (--)       Vehicle body to World rotation matrix. */
    double Rdot[3][3];
    double angular_velocity[3];
    double I_body[3][3];             /* (kg*m2)    Inertia Tensor in Body frame. */
    double I_body_inverse[3][3];

    double momentum[3];              /* (kg*m/s)   Linear Momentum */
    double force_total[3];
    double force_gravity[3];
    double force_buoyancy[3];
    double force_drag[3];

    double position[3];              // (m)   Position (world coordinates).
    double velocity[3];              // (m/s) Velocity (world coordinates).
    double mass_vehicle;             // (kg)  Vehicle mass

    double angular_momentum[3];      /* (kg*m2/s)  Angular Momentum */

    double torque_buoyancy[3];
    double torque_drag[3];
    double torque_total[3];

    double center_of_buoyancy[3];

    double volume_displaced;
    double mass_displaced;

    CrewModuleShape crewModuleShape;

    CrewModuleDynamics();

    void init_defaults();
    void calcVolumeAndCenterOfBuoyancy();
    void calcVelocity();
    void calcRDot();
    void calcAngularVelocity();

    void calcTotalTorque();
    void calcBuoyancyTorque();
    void calcDragTorque();

    void calcMassDisplaced();
    
    void calcTotalForce();
    void calcBuoyancyForce();
    void calcGravityForce();
    void calcDragForce();

    int calc_derivatives();
    int calc_state();

};
#endif

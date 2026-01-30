/************************************************************************
PURPOSE: (Simulate a ... .)
LIBRARY DEPENDENCIES:
    ((CrewModule/src/CrewModuleDynamics.o)((CrewModule/src/CrewModuleShape.o)))
**************************************************************************/
#ifndef CREW_MODULE_DYNAMICS_HH
#define CREW_MODULE_DYNAMICS_HH

#include "../include/CrewModuleShape.hh"

class CrewModuleDynamics
{
public:
    double R[3][3]; /* (--)       Vehicle body to World rotation matrix. */
    double Rdot[3][3];
    double angular_velocity[3];
    double I_body[3][3]; /* (kg*m2)    Inertia Tensor in Body frame. */
    double I_body_inverse[3][3];

    double momentum[3];       /* (N*s)   Linear Momentum */
    double force_total[3];    /* (N) */
    double force_gravity[3];  /* (N) */
    double force_buoyancy[3]; /* (N) */
    double force_drag[3];     /* (N) */

    double position[3];  // (m)   Position (world coordinates).
    double velocity[3];  // (m/s) Velocity (world coordinates).
    double mass_vehicle; // (kg)  Vehicle mass

    double angular_momentum[3]; /* (kg*m2/s)  Angular Momentum */

    double torque_buoyancy[3]; /* (N*m) */
    double torque_drag[3];     /* (N*m) */
    double torque_total[3];    /* (N*m) */

    double center_of_buoyancy[3];

    double volume_displaced; // (m^3)
    double mass_displaced;   // (kg) mass of displaced water.

    CrewModuleShape crewModuleShape;

    double density_of_water; // (kg/m^3) density of water.

    CrewModuleDynamics();

    void init_defaults();
    void init_inertia_tensor(double A, double B, double C);
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

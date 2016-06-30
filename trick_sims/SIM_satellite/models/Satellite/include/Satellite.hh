#ifndef SATELLITE_HH
#define SATELLITE_HH
/********************************* TRICK HEADER *******************************
PURPOSE:
      ()
LIBRARY DEPENDENCY:
      ((../src/Satellite.cpp))
PROGRAMMERS:
      (((John M. Penn) () () () ))
*******************************************************************************/

#include "Thruster.hh"
#include "Planet.hh"

class Satellite {

    public:
    Satellite() {}
    Satellite( Planet* planet);

    void initialize( Planet* planet);

    void compute_state_deriv();
    int  compute_state();
    void print_state();
    void compute_auxiliary_vars();
    void compute_platform_to_world();

    void get_Force_and_Torque();
    void get_force_of_gravity (double* gforcev);

    double mass;             /* (kg) */
    double Ibody[3][3];      /* (kg*m2)    Inertia Tensor in Body frame. */
    double Ibodyinv[3][3];   /* (kg*m2)    Inertia Tensor body inverse. */

    // State Variables
    double pos[3];           /* (m)        Vehicle position in world frame. */
    double R[3][3];          /* (--)       Vehicle body to World rotation matrix. */
    double P[3];             /* (kg*m/s)   Linear Momentum */
    double L[3];             /* (kg*m2/s)  Angular Momentum */

    // Aux Variables (Computed, given State variables)
    double Iinv[3][3];       /* (--) */
    double Rdot[3][3];       /* (--) */
    double vel[3];           /* (m/s)      Vehicle velocity in world frame. */
    double omega[3];         /* (rad/s)      Angular velocity. */
    double Rplat[3][3];      /* (--)       Vehicle platform to World rotation matrix. */

    double force[3];         /* (kg*m/s2)  Force on vehicle. */
    double torque[3];        /* (kg*m2/s2) Torque on vehicle. */

    Planet* planet;          /* (--) */

    Thruster main_engine;

    Thruster thruster_T1;
    Thruster thruster_T2;
    Thruster thruster_T3;
    Thruster thruster_T4;

    Thruster thruster_R1;
    Thruster thruster_R2;
    Thruster thruster_R3;
    Thruster thruster_R4;

    Thruster thruster_B1;
    Thruster thruster_B2;
    Thruster thruster_B3;
    Thruster thruster_B4;

    Thruster thruster_L1;
    Thruster thruster_L2;
    Thruster thruster_L3;
    Thruster thruster_L4;

};
#endif

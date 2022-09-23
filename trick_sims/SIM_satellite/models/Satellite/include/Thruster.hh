#ifndef THRUSTER_HH
#define THRUSTER_HH
/********************************* TRICK HEADER *******************************
PURPOSE:
      ()
LIBRARY DEPENDENCY:
      ((../src/Thruster.cpp))
PROGRAMMERS:
      (((John M. Penn) () () () ))
*******************************************************************************/

class Thruster {
    public:
    Thruster();
    void add_force_and_torque(double* force, double* torque);
    void initialize(double p0, double p1, double p2, double f0, double f1, double f2);


    bool   on;                   /* (--)      Whether the thruster is on (true) or off (false). */
    double position[3];          /* (m)       Position of the thruster in vehicle body frame. */
    double thrust[3];            /* (kg*m/s2) Thrusting force in vehicle body frame */
};
#endif

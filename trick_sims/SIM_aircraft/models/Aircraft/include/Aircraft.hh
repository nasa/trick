/************************************************************************
PURPOSE: (Simulate an Aircraft.)
LIBRARY DEPENDENCIES:
    ((Aircraft/src/Aircraft.o))
**************************************************************************/
#ifndef Aircraft_HH
#define Aircraft_HH

class Aircraft {
    public:
    // Calculated by numeric integration of state derivatives.
  	double pos[2];
  	double vel[2];

    // Updated by control logic;
    int current_waypoint;

    // Static Parameters (Don't change during the simulation.)
  	double mass;
    double thrust_mag_max;
  	double K_drag;
    double heading_rate_gain;

    // Control Parameters (Recieved from the variable server client.)
    double desired_heading;
    double desired_speed;   // m/s
    bool   autoPilot;

    // Calculated
    // Input to numeric integrator.
  	double acc[2];
    //
    double heading;

    // Methods
    int default_data();
    int state_init();
    int state_deriv();
    int state_integ();
    int control();

    void set_desired_compass_heading( double compass_heading);

    void calc_total_force( double (&F_total_body)[2]);
    void calc_drag_force( double (&F_drag_body)[2]);
    void calc_thrust_force(double (&F_thrust_body)[2]);
    void calc_turning_force(double (&F_turning_body)[2]);
    void rotateBodyToWorld(double (&F_total_world)[2], double (&F_total_body)[2] );

};

double psiToCompass( double psi );
double compassToPsi( double compass_heading );
double northWestToPsi (double (&p)[2]) ;
double vector_magnitude(double (&vector)[2]);
void vector_difference(double (&result)[2], double (&A)[2], double (&B)[2] );
//

#endif

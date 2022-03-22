/************************************************************************
PURPOSE: (Simulate a hor-air balloon.)
LIBRARY DEPENDENCIES:
    ((balloon/src/Balloon.o))
**************************************************************************/
#ifndef BALLOON_HH
#define BALLOON_HH

class Balloon {
    public:
    // State Variables (Uncalculated Variables)
	double pos[2];
	double vel[2];
	double envelope_mass;
	double basket_mass;
	double burner_system_mass;
	double payload_mass;
	double envelope_air_temperature;
	double envelope_radius;
	double envelope_theta;
	double wind_speed;
	double Cd[2];

    // Calculated Variables
	double acc[2];
	double envelope_volume;
	double fixed_mass;

    // Control Variable
    int temperature_change_command;
    int wind_change_command;

    // Methods
    int default_data();
    int state_init();
    int state_deriv();
    int state_integ();
    int check_ground_contact();
    int control();
    double calc_fixed_mass();
    double calc_envelope_volume();
    double calc_total_mass();
    double calc_envelope_air_mass();
    double calc_heated_air_density();
    double calc_buoyancy_force();
    double calc_displaced_air_mass();
    void calc_drag_force(double * F);
    double volume_of_a_spherical_dome( double r, double h);
    double volume_of_a_cone( double r, double h);
};
#endif

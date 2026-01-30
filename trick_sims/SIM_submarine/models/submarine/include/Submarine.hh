/************************************************************************
PURPOSE: (Simulate a submarine.)
LIBRARY DEPENDENCIES:
    ((submarine/src/Submarine.o))
**************************************************************************/
#ifndef SUBMARINE_HH
#define SUBMARINE_HH

// Density of salt water = 1023.6 kg/m^3
#define DENSITY_SALT_WATER 1023.6
// Acceleration of gravity = 9.80665 m/s^2
#define GRAVITY 9.80665
//
#define CONVERSION_PASCALS_TO_PSI 1.4504e-4

class Submarine
{
public:
    // Parameters
    double outer_hull_outer_radius;
    double outer_hull_inner_radius;
    double inner_hull_outer_radius;
    double inner_hull_inner_radius;
    double hull_length;
    double ballast_tank_volume;
    double hard_ballast_mass;
    double hull_mass;
    double payload_mass;
    double Cd;

    // State Variables (Uncalculated Variables)
    double pos[2];
    double vel[2];
    double ballast_energy; // joules
    double pump_power;     // watts

    // Calculated Variables
    double acc[2];

    // Control Variable
    double ballast_air_ratio; // To the GUI.
    double water_pressure_pascals;
    double water_pressure_PSI; // To the GUI.
    int pump_power_command;    // From the GUI.

    // Methods
    int default_data();
    int state_init();
    int state_deriv();
    int state_integ();
    int control();
    void calc_water_pressure();
    double calc_ballast_air_volume();
    double calc_pressure_hull_displaced_volume();
    double calc_total_displaced_volume();
    double calc_gravity_force();
    double calc_total_mass();
    double calc_fixed_mass();
    double calc_buoyancy_force();
    double calc_ballast_water_mass();
    double calc_ballast_water_volume();
    double calc_drag_force();
    double calc_pressure_hull_volume();
    int state_post_integ();
};
#endif

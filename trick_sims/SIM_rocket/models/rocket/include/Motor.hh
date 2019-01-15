/*************************************************************************
PURPOSE: (Simulate a model rocket motor.)
LIBRARY DEPENDENCIES:
    ((rocket/src/Motor.o)
    (rocket/src/interpolate.o))
**************************************************************************/
class Motor {
public:
    const double * thrust_sample_times;
    const double * thrust_sample_values;
    unsigned int thrust_sample_count;

    double total_impulse;   /* kg*m/s^2 */
    double total_mass;      /* kg */
    double propellant_mass; /* kg */
    double exhaust_speed;   /* m/s */

    int default_data ();
    int state_init ();
    double thrust_magnitude(double time);
};

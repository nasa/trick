/*************************************************************************
PURPOSE: (Environment for Model Rocket Sim)
LIBRARY DEPENDENCIES:
    ((rocket/src/Environment.o))
**************************************************************************/
class Environment {
public:
    double air_density; /* kg/m^3 */
    double gravity[2];  /* m/s^2 */
    int default_data();
};

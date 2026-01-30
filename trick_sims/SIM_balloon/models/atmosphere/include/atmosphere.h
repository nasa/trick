#ifndef ATMOSPHERE_H
#define ATMOSPHERE_H

#ifdef __cplusplus
extern "C"
{
#endif

    /* Returns atmospheric density (kg/m^3) at the given altitude (m).*/
    double US_STD_density(double alt_m);

    /* Returns acceleration of gravity (m/s^2) at the given altitude (m).*/
    double US_STD_gravity(double alt_m);

    /* Returns atmospheric temperature (℃) at the given altitude (m).*/
    double US_STD_temperature(double alt_m);

    /* Returns atmospheric pressure (pascals) at the given altitude (m).*/
    double US_STD_pressure(double alt_m);

#ifdef __cplusplus
}
#endif

#endif

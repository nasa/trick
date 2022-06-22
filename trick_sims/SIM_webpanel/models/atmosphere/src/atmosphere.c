#include <stdio.h>

static double interpolate( double x, const double xa[], const double fa[], int N_elems ) {
    int ii;
    for (ii=0 ; ((ii+2 < N_elems ) && (x > xa[ii+1])) ; ii++ ) ;
    double x_lower = xa[ii];
    double x_upper = xa[ii+1];
    double f_lower = fa[ii];
    double f_upper = fa[ii+1];
    if (x < x_lower) {
        fprintf(stderr, "Interpolation x is out of range low.\n");
        return(f_lower);
    }
    if (x > x_upper) {
        fprintf(stderr, "Interpolation x is out of range high.\n");
        return(f_upper);
    }
    double f = (x_upper - x)/(x_upper - x_lower) * f_lower +
               (x - x_lower)/(x_upper - x_lower) * f_upper ;
    return(f);
}

#define NUM_ELEMENTS 21
// Units = meters (above sea level).
const double altitude_array[NUM_ELEMENTS] = {
 -1000.0,     0.0,  1000.0,  2000.0,  3000.0,  4000.0,  5000.0,  6000.0,
  7000.0,  8000.0,  9000.0, 10000.0, 15000.0, 20000.0, 25000.0, 30000.0,
 40000.0, 50000.0, 60000.0, 70000.0, 80000.0 };

// Units = kilograms per cubic meter.
const double US_STD_density_array[NUM_ELEMENTS] = {
    1.347, 1.225, 1.112, 1.007, 0.9093, 0.8194, 0.7364, 0.6601,
   0.5900, 0.5258, 0.4671, 0.4135, 0.1948, 0.08891, 0.04008, 0.01841,
 0.003996, 0.001027, 0.0003097, 0.00008283, 0.00001846 };

// Units = meters per second squared.
const double US_STD_gravity_array[NUM_ELEMENTS] = {
9.810, 9.807, 9.804, 9.801, 9.797, 9.794, 9.791, 9.788,
9.785, 9.782, 9.779, 9.776, 9.761, 9.745, 9.730, 9.715,
9.684, 9.654, 9.624, 9.594, 9.564
};

// Units = celsius
const double US_STD_temperature_array[NUM_ELEMENTS] = {
 21.50,  15.00,   8.50,   2.00,  -4.49, -10.98, -17.47, -23.96,
-30.45, -36.94, -43.42, -49.90, -56.50, -56.50, -51.60, -46.64,
-22.80,   -2.5, -26.13, -53.57, -74.51
};

// Units = pascals
const double US_STD_pressure_array[NUM_ELEMENTS] = {
113900.0, 101325.0, 89880.0, 79500.0, 70120.0, 61660.0, 54050.0, 47220.0,
 41110.0,  35650.0, 30800.0, 26500.0, 12110.0,  5529.0,  2549.0,  1197.0,
  287.00,    79.78,   21.96,    5.20,    1.10
};

double US_STD_density ( double alt_m ) {
    return interpolate( alt_m, altitude_array, US_STD_density_array, NUM_ELEMENTS );
}

double US_STD_gravity( double alt_m) {
    return interpolate( alt_m, altitude_array, US_STD_gravity_array, NUM_ELEMENTS );
}

double US_STD_temperature( double alt_m ) {
    return interpolate( alt_m, altitude_array, US_STD_temperature_array, NUM_ELEMENTS );
}

double US_STD_pressure( double alt_m) {
    return interpolate( alt_m, altitude_array, US_STD_pressure_array, NUM_ELEMENTS );
}

#include <math.h>
#include <stdio.h>
#include "SAIntegrator.hh"

#define GRAVITATIONAL_CONSTANT 6.674e-11
#define EARTH_MASS 5.9723e24
#define EARTH_RADIUS 6367500.0

struct Orbit {
    double pos[2];
    double vel[2];
    double planet_mass;
};
void init_state ( Orbit& orbit ) {
    orbit.pos[0] = 0.0;
    orbit.pos[1] = 6578000.0;
    orbit.vel[0] = 7905.0;
    orbit.vel[1] = 0.0;
    orbit.planet_mass = EARTH_MASS;
}
void print_header() {
    printf ("time, orbit.pos[0], orbit.pos[1], orbit.vel[0], orbit.vel[1]\n");
}
void print_state( double t, Orbit& orbit ) {
    printf ("%10.10f, %10.10f, %10.10f, %10.10f, %10.10f\n",
             t, orbit.pos[0], orbit.pos[1], orbit.vel[0], orbit.vel[1]);
}
void G( double t, double x[], double g_out[], void* udata) {
    Orbit* orbit = (Orbit*)udata;
    double d = sqrt( x[0]*x[0] + x[1]*x[1]);
    g_out[0] = -x[0] * GRAVITATIONAL_CONSTANT * orbit->planet_mass / (d*d*d);
    g_out[1] = -x[1] * GRAVITATIONAL_CONSTANT * orbit->planet_mass / (d*d*d);
}
void F( double t, double v[], double f_out[], void* udata) {
    f_out[0] = v[0];
    f_out[1] = v[1];
}
int main ( int argc, char* argv[]) {
    Orbit orbit;
    double* x_p[2] = { &orbit.pos[0], &orbit.pos[1] };
    double* v_p[2] = { &orbit.vel[0], &orbit.vel[1] };
    unsigned count = 0;
    double dt = 0.1;
    double time = count * dt;
    init_state(orbit);
    print_header();
    print_state(time, orbit);
    SA::EulerCromerIntegrator integ(dt, 2, x_p, v_p, G, F, &orbit);
    while (time < 6000) {
        integ.load();
        integ.step();
        integ.unload();
        count ++;
        time = count * dt;
        print_state(time, orbit);
    }
}

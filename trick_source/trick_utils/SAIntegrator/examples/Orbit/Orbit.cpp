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
void G( double t, double pos[], double vel[], double acc_out[], void* udata) {
    Orbit* orbit = (Orbit*)udata;
    double d = sqrt( pos[0]*pos[0] + pos[1]*pos[1]);
    acc_out[0] = -pos[0] * GRAVITATIONAL_CONSTANT * orbit->planet_mass / (d*d*d);
    acc_out[1] = -pos[1] * GRAVITATIONAL_CONSTANT * orbit->planet_mass / (d*d*d);
}
int main ( int argc, char* argv[]) {
    Orbit orbit;
    double time = 0.0;
    double* x_p[2] = { &orbit.pos[0], &orbit.pos[1] };
    double* v_p[2] = { &orbit.vel[0], &orbit.vel[1] };
    double dt = 0.1;
    init_state(orbit);
    print_header();
    print_state(time, orbit);
    SA::EulerCromerIntegrator integ(dt, 2, x_p, v_p, G, &orbit);
    while (time < 5600) {
        integ.integrate();
        time = integ.getIndyVar();
        print_state(time, orbit);
    }
}

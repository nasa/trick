#include <math.h>
#include <stdio.h>
#include "SAIntegrator.hh"
#define N_DIMENSIONS 1
struct MassSpringDamper {
    double pos;
    double vel;
    double k;     // Spring constant
    double c;     // Damping constant
    double mass;
};
void init_state ( MassSpringDamper& msd ) {
    msd.pos = 2.0;
    msd.vel = 0.0;
    msd.k = 1.0;
    msd.c = 0.0;
    msd.mass = 5.0;
}
void print_header() {
    printf ("time, msd.pos, msd.vel\n");
}
void print_state( double t, MassSpringDamper& msd ) {
    printf ("%10.10f, %10.10f, %10.10f\n", t, msd.pos, msd.vel);
}
void G( double t, double x[], double g_out[], void* udata) {
    MassSpringDamper* msd = (MassSpringDamper*)udata;
    g_out[0] = -(msd->k/msd->mass) * x[0];
}
void F( double t, double v[], double f_out[], void* udata) {
    f_out[0] = v[0];
    f_out[1] = v[1];
}
int main ( int argc, char* argv[]) {
    MassSpringDamper msd;
    double* x_p[N_DIMENSIONS] = { &msd.pos };
    double* v_p[N_DIMENSIONS] = { &msd.vel };
    unsigned count = 0;
    double dt = 0.001;
    double time = count * dt;
    init_state(msd);
    print_header();
    print_state(time, msd);
    SA::EulerCromerIntegrator integ(dt, N_DIMENSIONS, x_p, v_p, G, F, &msd);
    while (time < 50) {
        integ.load();
        integ.step();
        integ.unload();
        count ++;
        time = count * dt;
        print_state(time, msd);
    }
}

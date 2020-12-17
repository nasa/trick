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
    msd.c = 0.2;
    msd.mass = 5.0;
}
void print_header() {
    printf ("time, msd.pos, msd.vel\n");
}
void print_state( double t, MassSpringDamper& msd ) {
    printf ("%10.10f, %10.10f, %10.10f\n", t, msd.pos, msd.vel);
}
void G( double t, double x[], double v[], double g_out[], void* udata) {
    MassSpringDamper* msd = (MassSpringDamper*)udata;
    g_out[0] = -(msd->k/msd->mass) * x[0]
               -(msd->c/msd->mass) * v[0];
}
int main ( int argc, char* argv[]) {
    MassSpringDamper msd;
    double time = 0.0;
    double* x_p[N_DIMENSIONS] = { &msd.pos };
    double* v_p[N_DIMENSIONS] = { &msd.vel };
    double dt = 0.001;
    init_state(msd);
    print_header();
    print_state(time, msd);
    SA::EulerCromerIntegrator integ(dt, N_DIMENSIONS, x_p, v_p, G, &msd);
    while (time < 50) {
        integ.integrate();
        time = integ.getIndyVar();
        print_state(time, msd);
    }
}

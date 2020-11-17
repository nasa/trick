#include <math.h>
#include <stdio.h>
#include <iostream>
#include "SAIntegrator.hh"

struct Cannon {
    double pos[2];
    double vel[2];
    double acc[2];
};
void init_state ( Cannon& cannon ) {
    cannon.pos[0] = 0.0;
    cannon.pos[1] = 0.0;
    cannon.vel[0] = 50.0 * cos(M_PI/6.0);
    cannon.vel[1] = 50.0 * sin(M_PI/6.0);
    cannon.acc[0] = 0.0;
    cannon.acc[1] = -9.81;
}
void calc_derivs( double t, double state[], double derivs[], void* udata) {
    derivs[0] = state[2]; // vel_x
    derivs[1] = state[3]; // vel_y
    derivs[2] = state[4]; // acc_x
    derivs[3] = state[5]; // acc_y
    derivs[4] = 0.0;      // Zero acc rate of change
    derivs[5] = 0.0;
}
double impact( double t, double state[], RootFinder* root_finder, void* udata) {
    double root_error = root_finder->find_roots(t, state[1]);
    if (root_error == 0.0) {
        root_finder->init();
        state[2] =  0.9 * state[2];
        state[3] = -0.9 * state[3];
    }
    return (root_error);
}
void print_header() {
    printf ("t, cannon.pos[0], cannon.pos[1], cannon.vel[0], cannon.vel[1], cannon.acc[0], cannon.acc[1]\n");
}
void print_state( double t, Cannon& cannon) {
    printf ("%5.3f, %5.10f, %5.10f, %5.10f, %5.10f, %5.10f, %5.10f\n",
    t, cannon.pos[0], cannon.pos[1], cannon.vel[0], cannon.vel[1], cannon.acc[0], cannon.acc[1]);
}
int main ( int argc, char* argv[]) {
    Cannon cannon;
    double* state_var_p[6] = { &(cannon.pos[0]), &(cannon.pos[1]),
                               &(cannon.vel[0]), &(cannon.vel[1]),
                               &(cannon.acc[0]), &(cannon.acc[1]) };
    double dt = 0.01;
    double t = 0.0;
    SA::RK2Integrator integ(dt, 6, state_var_p, state_var_p, calc_derivs, NULL);

    RootFinder root_finder(0.00000000001, Negative);
    integ.add_Rootfinder(&root_finder, &impact);

    init_state(cannon);
    print_header();
    print_state( t, cannon);
    while (t < 20.0) {
        integ.integrate();
        t = integ.getIndyVar();
        print_state( t, cannon);
    }
}

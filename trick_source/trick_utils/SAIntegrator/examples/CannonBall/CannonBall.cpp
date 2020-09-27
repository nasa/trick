#include <math.h>
#include <stdio.h>
#include "SAIntegrator.hh"

struct Cannon {
    double pos[2];
    double vel[2];
};
void init_state ( Cannon& cannon ) {
    cannon.pos[0] = 0.0;
    cannon.pos[1] = 0.0;
    cannon.vel[0] = 50.0 * cos(M_PI/6.0);
    cannon.vel[1] = 50.0 * sin(M_PI/6.0);
}
void deriv( double t, double state[], double derivs[], void* udata) {
    derivs[0] = state[2]; // vel_x
    derivs[1] = state[3]; // vel_y
    derivs[2] =  0.0;     // acc_x
    derivs[3] = -9.81;    // acc_y
}
void print_header() {
    printf ("t, cannon.pos[0], cannon.pos[1], cannon.vel[0], cannon.vel[1]\n");
}
void print_state( double t, Cannon& cannon) {
    printf ("%5.3f, %10.10f, %10.10f, %10.10f, %10.10f\n", t, cannon.pos[0], cannon.pos[1], cannon.vel[0], cannon.vel[1]);
}
int main ( int argc, char* argv[]) {
    Cannon cannon;
    double* state_var_p[4] = { &(cannon.pos[0]), &(cannon.pos[1]), &(cannon.vel[0]), &(cannon.vel[1]) };
    unsigned count = 0;
    double dt = 0.01;
    double t = count * dt;
    SA::RK2Integrator integ(dt, 4, state_var_p, state_var_p, deriv, NULL);
    init_state(cannon);
    print_header();
    print_state(t, cannon );
    while (t < 5.1) {
        integ.load();
        integ.step();
        integ.unload();
        count ++;
        t = count * dt;
        print_state(t, cannon );
    }
}

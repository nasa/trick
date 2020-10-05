#include <math.h>
#include <stdio.h>
#include "SAIntegrator.hh"

struct IContext {
    SA::RK2Integrator* integ;
    double start;
    double end;
};

void deriv_y( double y, double state_y[], double derivs_y[], void* udata) {
    double& x = state_y[0];
    derivs_y[0] = 0.0;           // (dx/dy) = 0, Hold x constant.
    derivs_y[1] = cos(x)*cos(y); // area = ∫ f(X,y) dy
}

void deriv_x( double x, double state_x[], double derivs_x[], void* udata) {
    IContext* context = (IContext*)udata;
    double area = 0.0;
    double* state_y[2] = { &x, &area };
    SA::RK2Integrator* integ_y = context->integ;
    integ_y->set_in_vars(state_y);
    integ_y->set_out_vars(state_y);
    integ_y->setIndyVar(context->start);
    while (integ_y->getIndyVar() <= context->end) {
        integ_y->load();
        integ_y->step();
        integ_y->unload();
    }
    derivs_x[0] = area; // volume = ∫ area dx
}

double doubleIntegral( double x_start, double x_end, double y_start, double y_end ) {
    double volume = 0.0;
    double dx = 0.01;
    double dy = 0.01;
    double* state_x[1] = { &volume };
    SA::RK2Integrator integ_y (dy, 2, NULL, NULL, deriv_y, NULL);
    IContext y_integ_context = {&integ_y, y_start, y_end};
    SA::RK2Integrator integ_x (dx, 1, state_x, state_x, deriv_x, &y_integ_context);
    integ_x.setIndyVar(x_start);
    while (integ_x.getIndyVar()<=x_end) {
        integ_x.load();
        integ_x.step();
        integ_x.unload();
    }
    return volume;
}

int main ( int argc, char* argv[]) {
    double vol = doubleIntegral( -M_PI/2.0, M_PI/2.0, -M_PI/2.0, M_PI/2.0 );
    printf("Volume = %g.\n", vol);
}

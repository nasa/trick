#include <gtest/gtest.h>
#include <iostream>
#include "SAIntegrator.hh"
#include <math.h>

void deriv4( double t, double state[], double derivs[], void* udata) {
    double t2 = t*t;
    double t3 = t2*t;

    derivs[0] = 3.0*t3 - 3.0*t2 - 3.0*t +  4.0;
    derivs[1] = 2.0*t3 + 1.0*t2 - 5.0*t +  7.0;
    derivs[2] =     t3 +     t2 + 5.0*t +  4.0;
    derivs[3] =     t3 - 6.0*t2 + 4.0*t + 12.0;
}

void deriv2( double t, double state[], double derivs[], void* udata) {

    derivs[0] = -2.0*t + 3.0;
    derivs[1] =  5.0*t + 4.0;
    derivs[2] =  3.0*t - 3.0;
    derivs[3] =  5.0*t + 4.0;
}

void deriv1( double t, double state[], double derivs[], void* udata) {

    derivs[0] = 4.0;
    derivs[1] = -4.0;
    derivs[2] = M_PI;
    derivs[3] = -M_PI;
}

#define EXCEPTABLE_ERROR 0.00000000001

TEST(SAIntegrator_unittest, EulerIntegrator_1) {

   double state[4] = {0.0, 0.0, 0.0, 0.0};
   double* state_var_p[4] = { &(state[0]), &(state[1]), &(state[2]), &(state[3])};
   double dt = 0.01;
   unsigned int count = 0;
   double t = count * dt;
   SA::EulerIntegrator integ(dt, 4, state_var_p, state_var_p, deriv1, NULL);
   while (t < 2.0) {
        integ.load();
        integ.step();
        integ.unload();
        count ++;
        t = count * dt;
    }
    EXPECT_NEAR(state[0], 8.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[1], -8.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[2], (2*M_PI), EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[3], (-2*M_PI), EXCEPTABLE_ERROR);
}

TEST(SAIntegrator_unittest, RungeKutta2_1) {

   double state[4] = {0.0, 0.0, 0.0, 0.0};
   double* state_var_p[4] = { &(state[0]), &(state[1]), &(state[2]), &(state[3]) };
   double dt = 0.01;
   unsigned int count = 0;
   double t = count * dt;
   SA::RK2Integrator integ(dt, 4, state_var_p, state_var_p, deriv2, NULL);
   while (t < 2.0) {
        integ.load();
        integ.step();
        integ.unload();
        count ++;
        t = count * dt;
    }
    EXPECT_NEAR(state[0], 2.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[1], 18.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[2], 0.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[3], 18.0, EXCEPTABLE_ERROR);
}

TEST(SAIntegrator_unittest, RungeKutta4_1) {

   double state[4] = {0.0, 0.0, 0.0, 0.0};
   double* state_var_p[4] = { &(state[0]), &(state[1]), &(state[2]), &(state[3]) };
   double dt = 0.01;
   unsigned int count = 0;
   double t = count * dt;
   SA::RK4Integrator integ(dt, 4, state_var_p, state_var_p, deriv4, NULL);
   while (t < 2.0) {
        integ.load();
        integ.step();
        integ.unload();
        count ++;
        t = count * dt;
    }
    EXPECT_NEAR(state[0], 6.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[1], (14.0+2/3.0), EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[2], (24+2/3.0), EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[3], 20.0, EXCEPTABLE_ERROR);
}

TEST(SAIntegrator_unittest, RungeKutta38_1) {

   double state[4] = {0.0, 0.0, 0.0, 0.0};
   double* state_var_p[4] = { &(state[0]), &(state[1]), &(state[2]), &(state[3]) };
   double dt = 0.01;
   unsigned int count = 0;
   double t = count * dt;
   SA::RK3_8Integrator integ(dt, 4, state_var_p, state_var_p, deriv4, NULL);
   while (t < 2.0) {
        integ.load();
        integ.step();
        integ.unload();
        count ++;
        t = count * dt;
    }
    EXPECT_NEAR(state[0], 6.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[1], (14.0+2/3.0), EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[2], (24+2/3.0), EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[3], 20.0, EXCEPTABLE_ERROR);
}

#include <gtest/gtest.h>
#include <iostream>
#include "SAIntegrator.hh"
#include <math.h>

#define EXCEPTABLE_GLOBAL_ERROR 0.00001
#define EXCEPTABLE_LOCAL_ERROR  0.000001

/* ========================================================
    RKF4(5) Basic Test
    Problem:  y' = y - x^2 + 1
              y(0) = 0.5
              for 0 <= x <= 2
    Solution: y = x^2 + 2*x + 1 - 0.5*e^x ; where x = 2.
              y = 5.30547.
   ========================================================
*/
void drv( double x, double state[], double derivs[], void* udata __attribute__((unused))) {
    double x2 = x*x;
    derivs[0] = state[0] - x2 + 1;
}
TEST(RKF45Integrator_unittest, basic_test) {
    double lower_limit =  0.0;
    double upper_limit =  2.0;
    double result;
    double h = 0.2;
    double* state[1] = { &result };
    SA::RKF45Integrator integ(EXCEPTABLE_LOCAL_ERROR, h, 1, state, drv, NULL);

    result = 0.5;
    integ.setIndyVar(lower_limit);
    double x = lower_limit;
    while (x < upper_limit) {
        integ.load();
        h = integ.adaptive_step(h);
        integ.unload();
        x = integ.getIndyVar();
        h = fmin(h,(upper_limit-x));
    }
    EXPECT_NEAR(result, 5.30547, EXCEPTABLE_GLOBAL_ERROR);
}

/* ========================================================
   Multi-Variable Test
   ========================================================
*/
void deriv4( double t,
             double state[] __attribute__((unused)),
             double derivs[],
             void* udata __attribute__((unused))) {
    double t2 = t*t;
    double t3 = t2*t;
    derivs[0] = 3.0*t3 - 3.0*t2 - 3.0*t +  4.0;
    derivs[1] = 2.0*t3 + 1.0*t2 - 5.0*t +  7.0;
    derivs[2] =     t3 +     t2 + 5.0*t +  4.0;
    derivs[3] =     t3 - 6.0*t2 + 4.0*t + 12.0;
}

TEST(RKF45Integrator_unittest, multi_var_test) {
    double lower_limit =  0.0;
    double upper_limit =  2.0;
    double state[4] = {0.0, 0.0, 0.0, 0.0};
    double* state_var_p[4] = { &(state[0]), &(state[1]), &(state[2]), &(state[3]) };
    double h = 0.2;
    SA::RKF45Integrator integ(EXCEPTABLE_LOCAL_ERROR, h, 4, state_var_p, deriv4, NULL);

    integ.setIndyVar(lower_limit);
    double x = lower_limit;
    while (x < upper_limit) {
        integ.load();
        h = integ.adaptive_step(h);
        integ.unload();
        x = integ.getIndyVar();
        h = fmin(h,(upper_limit-x));
    }
    EXPECT_NEAR(state[0], 6.0, EXCEPTABLE_GLOBAL_ERROR);
    EXPECT_NEAR(state[1], (14.0+2/3.0), EXCEPTABLE_GLOBAL_ERROR);
    EXPECT_NEAR(state[2], (24+2/3.0), EXCEPTABLE_GLOBAL_ERROR);
    EXPECT_NEAR(state[3], 20.0, EXCEPTABLE_GLOBAL_ERROR);
}

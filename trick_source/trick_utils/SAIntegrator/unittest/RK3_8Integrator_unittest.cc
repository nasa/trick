#include <gtest/gtest.h>
#include <iostream>
#include "SAIntegrator.hh"
#include <math.h>

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

#define EXCEPTABLE_ERROR 0.00000000001

TEST(FirstOrderODEVariableStepIntegrator_unittest, RungeKutta38_1) {

   double state[4] = {0.0, 0.0, 0.0, 0.0};
   double* state_var_p[4] = { &(state[0]), &(state[1]), &(state[2]), &(state[3]) };
   double dt = 0.01;
   unsigned int count = 0;
   double t = count * dt;
   SA::RK3_8Integrator integ1(dt, 4, state_var_p, state_var_p, deriv4, NULL);
   while (t < 2.0) {
        integ1.load();
        integ1.step();
        integ1.unload();
        count ++;
        t = count * dt;
    }
    EXPECT_NEAR(state[0], 6.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[1], (14.0+2/3.0), EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[2], (24+2/3.0), EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[3], 20.0, EXCEPTABLE_ERROR);

    // Use the copy-constructor to create create a duplicate RK3_8Integrator.
    SA::RK3_8Integrator integ2(integ1);

    // Create a text representation for each of the RK3_8Integrator.
    std::stringstream ss1;
    std::stringstream ss2;
    ss1 << integ1 << std::endl;
    ss2 << integ2 << std::endl;

    // Compare the text representations. They should be identical.
    int result = ss1.str().compare(ss2.str());
    EXPECT_EQ(result, 0);
}

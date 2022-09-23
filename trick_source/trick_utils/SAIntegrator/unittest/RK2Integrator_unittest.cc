#include <gtest/gtest.h>
#include <iostream>
#include "SAIntegrator.hh"
#include <math.h>

void deriv2( double t,
             double state[] __attribute__((unused)),
             double derivs[],
             void* udata __attribute__((unused))) {

    derivs[0] = -2.0*t + 3.0;
    derivs[1] =  5.0*t + 4.0;
    derivs[2] =  3.0*t - 3.0;
    derivs[3] =  5.0*t + 4.0;
}

#define EXCEPTABLE_ERROR 0.00000000001

TEST(RK2Integrator_unittest, test_load_step_unload) {

   double state[4] = {0.0, 0.0, 0.0, 0.0};
   double* state_var_p[4] = { &(state[0]), &(state[1]), &(state[2]), &(state[3]) };
   double dt = 0.01;
   unsigned int count = 0;
   double t = count * dt;
   SA::RK2Integrator integ1(dt, 4, state_var_p, state_var_p, deriv2, NULL);
   while (t < 2.0) {
        integ1.load();
        integ1.step();
        integ1.unload();
        count ++;
        t = count * dt;
    }
    EXPECT_NEAR(state[0], 2.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[1], 18.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[2], 0.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[3], 18.0, EXCEPTABLE_ERROR);
}

TEST(RK2Integrator_unittest, test_integrate) {

   double state[4] = {0.0, 0.0, 0.0, 0.0};
   double* state_var_p[4] = { &(state[0]), &(state[1]), &(state[2]), &(state[3]) };
   double dt = 0.01;
   unsigned int count = 0;
   double t = count * dt;
   SA::RK2Integrator integ1(dt, 4, state_var_p, state_var_p, deriv2, NULL);
   while (t < 2.0) {
        integ1.integrate();
        count ++;
        t = count * dt;
    }
    EXPECT_NEAR(state[0], 2.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[1], 18.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[2], 0.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(state[3], 18.0, EXCEPTABLE_ERROR);
}



TEST(RK2Integrator_unittest, copy_constructor) {

   double state[4] = {0.0, 0.0, 0.0, 0.0};
   double* state_var_p[4] = { &(state[0]), &(state[1]), &(state[2]), &(state[3]) };
   double dt = 0.01;
   unsigned int count = 0;
   double t = count * dt;
   SA::RK2Integrator integ1(dt, 4, state_var_p, state_var_p, deriv2, NULL);
   while (t < 2.0) {
        integ1.integrate();
        count ++;
        t = count * dt;
    }

    // Use the copy-constructor to create create a duplicate RK2Integrator.
    SA::RK2Integrator integ2(integ1);

    // Create a text representation for each of the RK2Integrators.
    std::stringstream ss1;
    std::stringstream ss2;
    ss1 << integ1 << std::endl;
    ss2 << integ2 << std::endl;

    // Compare the text representations. They should be identical.
    int result = ss1.str().compare(ss2.str());
    EXPECT_EQ(result, 0);
}

TEST(RK2Integrator_unittest, assignment_operator) {

   double state[4] = {0.0, 0.0, 0.0, 0.0};
   double* state_var_p[4] = { &(state[0]), &(state[1]), &(state[2]), &(state[3]) };
   double dt = 0.01;
   unsigned int count = 0;
   double t = count * dt;
   SA::RK2Integrator integ1(dt, 4, state_var_p, state_var_p, deriv2, NULL);
   while (t < 2.0) {
        integ1.integrate();
        count ++;
        t = count * dt;
    }

    // Use the assignment operator to create create a duplicate RK2Integrator.
    SA::RK2Integrator integ2;
    integ2 = integ1;

    // Create a text representation for each of the RK2Integrators.
    std::stringstream ss1;
    std::stringstream ss2;
    ss1 << integ1 << std::endl;
    ss2 << integ2 << std::endl;

    // Compare the text representations. They should be identical.
    int result = ss1.str().compare(ss2.str());
    EXPECT_EQ(result, 0);
}

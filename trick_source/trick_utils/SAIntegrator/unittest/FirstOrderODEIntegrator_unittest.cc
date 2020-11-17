#include <gtest/gtest.h>
#include <iostream>
#include "SAIntegrator.hh"
#include <math.h>

#define EXCEPTABLE_ERROR 0.00000000001

void calc_derivs( double x       __attribute__((unused)),
                  double state[] __attribute__((unused)),
                  double derivs[],
                  void*  udata   __attribute__((unused))) {
    derivs[0] = 1.0;
    derivs[1] = 1.0;
    derivs[2] = 1.0;
    derivs[3] = 1.0;
}

/*
The default integration algorithm for FirstOrderODEIntegrator is Euler integration.
Tests:
    * getIndyVar
    * integrate
        * load
        * step
        * unload
*/
TEST(FirstOrderODEIntegrator_unittest, test_1) {
    double h = 0.1;
    unsigned int N = 4;
    double vars[4] = {1.0, 2.0, 3.0, 4.0} ;
    double* varptrs[4] = {&vars[0], &vars[1], &vars[2], &vars[3]};
    double x;

    SA::FirstOrderODEIntegrator integ( h, N, varptrs, varptrs, calc_derivs, NULL);

    x = integ.getIndyVar();
    EXPECT_NEAR(x, 0.0, EXCEPTABLE_ERROR);

    integ.integrate();
    EXPECT_NEAR(vars[0], 1.1, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[1], 2.1, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[2], 3.1, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[3], 4.1, EXCEPTABLE_ERROR);

    x = integ.getIndyVar();
    EXPECT_NEAR(x, 0.1, EXCEPTABLE_ERROR);
}
/*
Tests:
    * getIndyVar
    * integrate
        * load
        * step
        * unload
    * undo_integrate
*/
TEST(FirstOrderODEIntegrator_unittest, test_2) {
    double h = 0.1;
    unsigned int N = 4;
    double vars[4] = {1.0, 2.0, 3.0, 4.0} ;
    double* varptrs[4] = {&vars[0], &vars[1], &vars[2], &vars[3]};
    double x;

    SA::FirstOrderODEIntegrator integ( h, N, varptrs, varptrs, calc_derivs, NULL);

    integ.integrate();
    EXPECT_NEAR(vars[0], 1.1, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[1], 2.1, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[2], 3.1, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[3], 4.1, EXCEPTABLE_ERROR);

    x = integ.getIndyVar();
    EXPECT_NEAR(x, 0.1, EXCEPTABLE_ERROR);

    integ.undo_integrate();
    EXPECT_NEAR(vars[0], 1.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[1], 2.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[2], 3.0, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[3], 4.0, EXCEPTABLE_ERROR);

    x = integ.getIndyVar();
    EXPECT_NEAR(x, 0.0, EXCEPTABLE_ERROR);
}

/*
Tests:
    * getIndyVar
    * integrate
        * load
        * step
        * unload
    * load_from_outState
*/
TEST(FirstOrderODEIntegrator_unittest, test_3) {
    double h = 0.1;
    unsigned int N = 4;
    double vars[4] = {1.0, 2.0, 3.0, 4.0};
    double* varptrs[4] = {&vars[0], &vars[1], &vars[2], &vars[3]};

    SA::FirstOrderODEIntegrator integ( h, N, varptrs, varptrs, calc_derivs, NULL);

    integ.integrate();
    for (int i=0; i<5 ; i++) {
        integ.load_from_outState();
        integ.step();
    }
    integ.unload();
    EXPECT_NEAR(vars[0], 1.6, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[1], 2.6, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[2], 3.6, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[3], 4.6, EXCEPTABLE_ERROR);

    double x = integ.getIndyVar();
    EXPECT_NEAR(x, 0.6, EXCEPTABLE_ERROR);
}

/*
Tests:
    * getIndyVar
    * integrate
        * load
        * step
        * unload
    * load_from_outState
    * set_out_vars
*/
TEST(FirstOrderODEIntegrator_unittest, test_4) {
    double h = 0.1;
    unsigned int N = 4;

    double vars[4] = {1.0, 2.0, 3.0, 4.0};
    double* varptrs[4] = {&vars[0], &vars[1], &vars[2], &vars[3]};
    double altvars[4];
    double* altvarptrs[4] = {&altvars[0], &altvars[1], &altvars[2], &altvars[3]};

    SA::FirstOrderODEIntegrator integ( h, N, varptrs, varptrs, calc_derivs, NULL);

    integ.integrate();
    for (int i=0; i<5 ; i++) {
        integ.load_from_outState();
        integ.step();
    }
    integ.set_out_vars( altvarptrs);
    integ.unload();
    EXPECT_NEAR(altvars[0], 1.6, EXCEPTABLE_ERROR);
    EXPECT_NEAR(altvars[1], 2.6, EXCEPTABLE_ERROR);
    EXPECT_NEAR(altvars[2], 3.6, EXCEPTABLE_ERROR);
    EXPECT_NEAR(altvars[3], 4.6, EXCEPTABLE_ERROR);

    integ.set_out_vars( varptrs);
    integ.unload();
    EXPECT_NEAR(vars[0], 1.6, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[1], 2.6, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[2], 3.6, EXCEPTABLE_ERROR);
    EXPECT_NEAR(vars[3], 4.6, EXCEPTABLE_ERROR);

    double x = integ.getIndyVar();
    EXPECT_NEAR(x, 0.6, EXCEPTABLE_ERROR);
}

#include <gtest/gtest.h>
#include <iostream>
#include "SAIntegrator.hh"
#include <math.h>

#define EXCEPTABLE_ERROR 0.00000000001

TEST(Integrator, test_getIndyVar_and_setIndyVar) {
    double h = 0.1;
    void* udata = NULL;
    double x;

    SA::Integrator  integ( h, udata);
    //Confirm that initial value of the independent variable is 0.0;
    x = integ.getIndyVar();
    EXPECT_NEAR(x, 0.0, EXCEPTABLE_ERROR);

    //Set the independent variable, and then confirm that it's the value we set.;
    integ.setIndyVar(3.0);
    x = integ.getIndyVar();
    EXPECT_NEAR(x, 3.0, EXCEPTABLE_ERROR);
}
TEST(Integrator, test_load_and_step) {
    double h = 0.1;
    void* udata = NULL;
    double x;

    SA::Integrator  integ( h, udata);
    integ.setIndyVar(4.0);

    integ.load();
    integ.step();
    x = integ.getIndyVar();
    EXPECT_NEAR(x, 4.1, EXCEPTABLE_ERROR);

    integ.load();
    integ.step();
    x = integ.getIndyVar();
    EXPECT_NEAR(x, 4.2, EXCEPTABLE_ERROR);
}

TEST(Integrator, test_integrate) {
    double h = 0.1;
    void* udata = NULL;
    double x;

    SA::Integrator  integ( h, udata);
    integ.setIndyVar(4.0);

    integ.integrate();
    x = integ.getIndyVar();
    EXPECT_NEAR(x, 4.1, EXCEPTABLE_ERROR);

    integ.integrate();
    x = integ.getIndyVar();
    EXPECT_NEAR(x, 4.2, EXCEPTABLE_ERROR);
}

TEST(Integrator, test_undo_integrate) {
    double h = 0.1;
    void* udata = NULL;
    double x;

    SA::Integrator  integ( h, udata);
    integ.setIndyVar(4.0);

    integ.integrate();
    x = integ.getIndyVar();
    EXPECT_NEAR(x, 4.1, EXCEPTABLE_ERROR);

    integ.undo_integrate();
    x = integ.getIndyVar();
    EXPECT_NEAR(x, 4.0, EXCEPTABLE_ERROR);
}

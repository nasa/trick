#include <gtest/gtest.h>
#include <iostream>
#include "Rootfinder.hh"
#include <math.h>

#define EXCEPTABLE_ERROR 0.00000000001

double func_negative_slope(double x) {
    // Root: x = 8.5
    return ( -0.5 * x + 4.25);
}

double func_positive_slope(double x) {
    // Root: x = 8.5
    return ( 0.5 * x + -4.25);
}

// If we immediately hit a root, without any history, we can't tell whether the
// slope is positive or negative. So, we can only "detect" the root if the rootfinder
// is unconstrained.
TEST(RootFinder_unittest, Root_with_no_history_unconstrained) {
    RootFinder* root_finder = new RootFinder ( 0.00000000001, Unconstrained );
    double root_error;
    root_error = root_finder->find_roots(8.5, func_negative_slope(8.5));
    EXPECT_NEAR(root_error, 0.0, EXCEPTABLE_ERROR);
}

TEST(RootFinder_unittest, Unconstrained_with_negative_slope_function) {
    RootFinder* root_finder = new RootFinder ( 0.00000000001, Unconstrained );
    double root_error;
    root_error = root_finder->find_roots(8, func_negative_slope(8));
    EXPECT_NEAR(root_error, DBL_MAX, EXCEPTABLE_ERROR);
    root_error = root_finder->find_roots(9, func_negative_slope(9));
    EXPECT_NEAR(root_error, -0.5, EXCEPTABLE_ERROR);
}

TEST(RootFinder_unittest, Unconstrained_with_positive_slope_function) {
    RootFinder* root_finder = new RootFinder ( 0.00000000001, Unconstrained );
    double root_error;
    root_error = root_finder->find_roots(8, func_positive_slope(8));
    EXPECT_NEAR(root_error, DBL_MAX, EXCEPTABLE_ERROR);
    root_error = root_finder->find_roots(9, func_positive_slope(9));
    EXPECT_NEAR(root_error, -0.5, EXCEPTABLE_ERROR);
}

TEST(RootFinder_unittest, Positive_constraint_with_negative_slope_function) {
    RootFinder* root_finder = new RootFinder ( 0.00000000001, Positive );
    double root_error;
    root_error = root_finder->find_roots(8, func_negative_slope(8));
    EXPECT_NEAR(root_error, DBL_MAX, EXCEPTABLE_ERROR);
    root_error = root_finder->find_roots(9, func_negative_slope(9));
    EXPECT_NEAR(root_error, DBL_MAX, EXCEPTABLE_ERROR);
}

TEST(RootFinder_unittest, Positive_constraint_with_positive_slope_function) {
    RootFinder* root_finder = new RootFinder ( 0.00000000001, Positive );
    double root_error;
    root_error = root_finder->find_roots(8, func_positive_slope(8));
    EXPECT_NEAR(root_error, DBL_MAX, EXCEPTABLE_ERROR);
    root_error = root_finder->find_roots(9, func_positive_slope(9));
    EXPECT_NEAR(root_error, -0.5, EXCEPTABLE_ERROR);
}

TEST(RootFinder_unittest, Negative_constraint_with_negative_slope_function) {
    RootFinder* root_finder = new RootFinder ( 0.00000000001, Negative );
    double root_error;
    root_error = root_finder->find_roots(8, func_negative_slope(8));
    EXPECT_NEAR(root_error, DBL_MAX, EXCEPTABLE_ERROR);
    root_error = root_finder->find_roots(9, func_negative_slope(9));
    EXPECT_NEAR(root_error, -0.5, EXCEPTABLE_ERROR);
}

TEST(RootFinder_unittest, Negative_constraint_with_positive_slope_function) {
    RootFinder* root_finder = new RootFinder ( 0.00000000001, Negative );
    double root_error;
    root_error = root_finder->find_roots(8, func_positive_slope(8));
    EXPECT_NEAR(root_error, DBL_MAX, EXCEPTABLE_ERROR);
    root_error = root_finder->find_roots(9, func_positive_slope(9));
    EXPECT_NEAR(root_error, DBL_MAX, EXCEPTABLE_ERROR);
}

TEST(RootFinder_unittest, serialize) {
    RootFinder* root_finder = new RootFinder ( 0.00000000001, Negative );
    double root_error;
    root_error = root_finder->find_roots(8, func_positive_slope(8));
    EXPECT_NEAR(root_error, DBL_MAX, EXCEPTABLE_ERROR);
    root_error = root_finder->find_roots(9, func_positive_slope(9));
    EXPECT_NEAR(root_error, DBL_MAX, EXCEPTABLE_ERROR);
}

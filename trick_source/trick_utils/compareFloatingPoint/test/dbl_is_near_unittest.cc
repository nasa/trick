
#include "trick/compareFloatingPoint.hh"
#include <float.h>
#include <gtest/gtest.h>
#include <iostream>
#include <math.h>

TEST(dbl_is_near_unittest, Simple_1)
{
    bool result;
    double A = 1.0;
    double B = 1.1;
    double tolerance = 0.2;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(dbl_is_near_unittest, Simple_2)
{
    bool result;
    double A = 1234.567891;
    double B = 1234.567882;
    double tolerance = 0.00001;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(dbl_is_near_unittest, Simple_3)
{
    bool result;
    double A = -1.562154;
    double B = 0.435837;
    double tolerance = 2.0;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(dbl_is_near_unittest, Simple_4)
{
    bool result;
    double A = -1.562154;
    double B = 0.435837;
    double tolerance = 1.8;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(dbl_is_near_unittest, Simple_5)
{
    bool result;
    double A = -1.562154;
    double B = -0.435837;
    double tolerance = 1.2;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(dbl_is_near_unittest, A_is_FP_NAN)
{
    bool result;
    double A = NAN;
    double B = 0.0;
    double tolerance = DBL_MAX;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(dbl_is_near_unittest, B_is_FP_NAN)
{
    bool result;
    double A = 0.0;
    double B = NAN;
    double tolerance = DBL_MAX;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(dbl_is_near_unittest, A_is_FP_INFINITE)
{
    bool result;
    double A = HUGE_VAL;
    double B = DBL_MAX;
    double tolerance = 2 * DBL_EPSILON * DBL_MAX;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(dbl_is_near_unittest, B_is_FP_INFINITE)
{
    bool result;
    double A = DBL_MAX;
    double B = HUGE_VAL;
    double tolerance = 2 * DBL_EPSILON * DBL_MAX;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(dbl_is_near_unittest, A_and_B_are_identical)
{
    // Tolerance is irrelavant because A and B are identical.
    bool result;
    double A = DBL_MIN;
    double B = DBL_MIN;
    double tolerance = 0.0;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(dbl_is_near_unittest, Within_MinimumTolerance)
{
    // The specified tolerance is < DBL_MIN/DBL_EPSILON, and so defaults to the
    // minimum. Since the difference between A and B is within the tolerance,
    // they are "near".
    bool result;
    double A = DBL_MIN;
    double B = 1.5 * DBL_MIN;
    double tolerance = 0.0;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(dbl_is_near_unittest, Exactly_Minimum_Tolerance)
{
    // The specified tolerance is < DBL_MIN/DBL_EPSILON, and so defaults to the
    // minimum (DBL_MIN/DBL_EPSILON). Since the difference between A and B is
    // exactly equal to the tolerance, they are "near".
    bool result;
    double A = 0.0;
    double B = DBL_MIN / DBL_EPSILON;
    double tolerance = 0.0;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(dbl_is_near_unittest, Greater_than_Minimum_Tolerance)
{
    // The specified tolerance is < DBL_MIN/DBL_EPSILON, and so defaults to the
    // minimum (DBL_MIN/DBL_EPSILON). Since the difference between A and B is
    // slightly greater than the tolerance, they are not "near".
    bool result;
    double A = 0.0;
    double B = DBL_MIN / DBL_EPSILON + DBL_MIN;
    double tolerance = 0.0;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(dbl_is_near_unittest, Tolerance_greater_than_minimum)
{
    // This test is like the previous, but specifies the tolerance to be slightly
    // larger than the minimum, so that A and B are near.
    bool result;
    double A = 0.0;
    double B = DBL_MIN / DBL_EPSILON + DBL_MIN;
    double tolerance = DBL_MIN / DBL_EPSILON + DBL_MIN;
    result = Trick::dbl_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(dbl_is_near_unittest, Tolerance_is_small_enough)
{
    // Is the tolerance small enough?
    double min_tolerance = DBL_MIN / DBL_EPSILON;
    const double planck_length = 1.6e-35;                 // meters
    const double speed_of_light = 2.99e8;                 // meters/second
    const double seconds_per_year = 3.1e7;                // seconds/year
    const double light_years_per_known_universe = 9.3e10; // lightyears/known_universe
    double size_of_known_universe = speed_of_light * seconds_per_year * light_years_per_known_universe;
    double universe_min_to_max_ratio = planck_length / size_of_known_universe;
    std::cout << "=========================================" << std::endl;
    std::cout << "minimum tolerance = " << min_tolerance << std::endl;
    std::cout << "planck_length / size_of_known_universe = " << universe_min_to_max_ratio << std::endl;
    std::cout << "=========================================" << std::endl;
    bool result = (min_tolerance < universe_min_to_max_ratio);
    EXPECT_TRUE(result);
}

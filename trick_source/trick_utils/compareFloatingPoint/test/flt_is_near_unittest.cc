
#include "trick/compareFloatingPoint.hh"
#include <float.h>
#include <gtest/gtest.h>
#include <iostream>
#include <math.h>

TEST(flt_is_near_unittest, Simple_1)
{
    bool result;
    float A = 1.0;
    float B = 1.1;
    float tolerance = 0.2;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(flt_is_near_unittest, Simple_2)
{
    bool result;
    float A = 1234.567891;
    float B = 1234.567882;
    float tolerance = 0.00001;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(flt_is_near_unittest, Simple_3)
{
    bool result;
    float A = -1.562154;
    float B = 0.435837;
    float tolerance = 2.0;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(flt_is_near_unittest, Simple_4)
{
    bool result;
    float A = -1.562154;
    float B = 0.435837;
    float tolerance = 1.8;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(flt_is_near_unittest, Simple_5)
{
    bool result;
    float A = -1.562154;
    float B = -0.435837;
    float tolerance = 1.2;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(flt_is_near_unittest, A_is_FP_NAN)
{
    bool result;
    float A = NAN;
    float B = 0.0;
    float tolerance = FLT_MAX;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(flt_is_near_unittest, B_is_FP_NAN)
{
    bool result;
    float A = 0.0;
    float B = NAN;
    float tolerance = FLT_MAX;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(flt_is_near_unittest, A_is_FP_INFINITE)
{
    bool result;
    float A = HUGE_VAL;
    float B = FLT_MAX;
    float tolerance = 2 * FLT_EPSILON * FLT_MAX;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(flt_is_near_unittest, B_is_FP_INFINITE)
{
    bool result;
    float A = FLT_MAX;
    float B = HUGE_VAL;
    float tolerance = 2 * FLT_EPSILON * FLT_MAX;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(flt_is_near_unittest, A_and_B_are_identical)
{
    // Tolerance is irrelavant because A and B are identical.
    bool result;
    float A = FLT_MIN;
    float B = FLT_MIN;
    float tolerance = 0.0;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(flt_is_near_unittest, Within_MinimumTolerance)
{
    // The specified tolerance is < FLT_MIN/FLT_EPSILON, and so defaults to the
    // minimum. Since the difference between A and B is within the tolerance,
    // they are "near".
    bool result;
    float A = FLT_MIN;
    float B = 1.5 * FLT_MIN;
    float tolerance = 0.0;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(flt_is_near_unittest, Exactly_Minimum_Tolerance)
{
    // The specified tolerance is < FLT_MIN/FLT_EPSILON, and so defaults to the
    // minimum (FLT_MIN/FLT_EPSILON). Since the difference between A and B is
    // exactly equal to the tolerance, they are "near".
    bool result;
    float A = 0.0;
    float B = FLT_MIN / FLT_EPSILON;
    float tolerance = 0.0;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(flt_is_near_unittest, Greater_than_Minimum_Tolerance)
{
    // The specified tolerance is < FLT_MIN/FLT_EPSILON, and so defaults to the
    // minimum (FLT_MIN/FLT_EPSILON). Since the difference between A and B is
    // slightly greater than the tolerance, they are not "near".
    bool result;
    float A = 0.0;
    float B = FLT_MIN / FLT_EPSILON + FLT_MIN;
    float tolerance = 0.0;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_FALSE(result);
}

TEST(flt_is_near_unittest, Tolerance_greater_than_minimum)
{
    // This test is like the previous, but specifies the tolerance to be slightly
    // larger than the minimum, so that A and B are near.
    bool result;
    float A = 0.0;
    float B = FLT_MIN / FLT_EPSILON + FLT_MIN;
    float tolerance = FLT_MIN / FLT_EPSILON + FLT_MIN;
    result = Trick::flt_is_near(A, B, tolerance);
    EXPECT_TRUE(result);
}

TEST(flt_is_near_unittest, PrintNumbers)
{
    // This isn't really a test. It's purpose is to print interesting values.
    bool result;
    float min_tolerance = FLT_MIN / FLT_EPSILON;
    std::cout << "Minimum tolerance = " << min_tolerance << std::endl;
    EXPECT_TRUE(true);
}

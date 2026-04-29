extern "C" {
#include <stdio.h>
#include <stdlib.h>
#include "trick/trick_math.h"
#include "trick/trick_optimization.h"
#include "trick/reference_frame.h"
}
#include <gtest/gtest.h>
using namespace std;

class PDIPOptimizationTest : public ::testing::Test {

   public:

      PDIPOptimizationTest() {}
      ~PDIPOptimizationTest() {}
      void SetUp() {}
      void TearDown() {}
};

TEST_F(PDIPOptimizationTest, pdip_solver)
{
    /*
        Problem
        Minimize X1 + 2X2
        subject to the constraints
        9.5X1 + 4X2 <= 97.5
        X1 <= 9
        X1+X2 >= 2
        X1 - 2X2 >= -20
        3X1 + 15X2 >= 15
        X1 >= 0, X2 >= 0

        Add slack variables to convert the problem into standard form, that is, convert all
        inequalities into equalities by adding slack variables and have only inequalities
        of the form Xi > 0. The problem can be restated as
        Minimize X1 + 2X2
        Constraints
        9.5X1 + 4X2 + X3 = 97.5
        X1 + X4 = 9
        X1 + X2 - X5 = 2
        X1 - 2X2 - X6 = -20
        3X1 + 15X2 - X7 = 15
        X1 >= 0, X2 >= 0, X3 >= 0, X4 >= 0, X5 >= 0, X6 >= 0, X7 >= 0
    */

    int result;
    double eqarr[5][7] = {{9.5, 4, 1, 0, 0, 0, 0}, 
                          {1, 0, 0, 1, 0, 0, 0},
                          {1, 1, 0, 0, -1, 0, 0},
                          {1, -2, 0, 0, 0, -1, 0},
                          {3, 15, 0, 0, 0, 0, -1}
                         };
    double *A[5] = {eqarr[0], eqarr[1], eqarr[2], eqarr[3], eqarr[4] };
    double b[5] = {97.5, 9, 2, -20, 15};
    double minfn[7] = {1, 2, 0, 0, 0, 0, 0};
    double xvals[7] = {3, 3, 3, 3, 3, 3, 3};
    double lmb[7] = { 1, 1, 1, 1, 1, 1, 1 };
    double nu[5] = {1, 1, 1, 1, 1};

    result = dPDIP_linear_solver(7, minfn, 5, A, b, xvals, lmb, nu, 0.01, 0.25, 0.7, 5, 1.1, 25, 20);
    EXPECT_NEAR(xvals[0], 1.2498, 0.0001);
    EXPECT_NEAR(xvals[1], 0.7502, 0.0001);
    EXPECT_LE(result, 25);
}

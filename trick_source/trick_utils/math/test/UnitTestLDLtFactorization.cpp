#include <gtest/gtest.h>
#include "trick/trick_math.h"
#include "trick/reference_frame.h"
using namespace std;

class LDLtFactorizationTest : public ::testing::Test {

   public:

      LDLtFactorizationTest() {}
      ~LDLtFactorizationTest() {}
      void SetUp() {}
      void TearDown() {}
};

TEST_F(LDLtFactorizationTest, ldlt_solver)
{

    /* Calculate L and D for A and solve  AX = b
                      [  2  -1   0 ]                [ 1 ]
                [A] = [ -1   2  -1 ]          [b] = [ 0 ]
                      [  0  -1   1 ]                [ 0 ]
    */

    int i, n=3;

    double **L;
    double D[n];
    double x[n];
    double y[n];
    double **A;

    A = (double**)malloc(sizeof(double *) * n);
    L = (double**)malloc(sizeof(double *) * n);
    for(i=0; i < n; i++) {
        A[i] = (double *)malloc(sizeof(double)*n);
        L[i] = (double *)malloc(sizeof(double)*n);
    }

    A[0][0] = 2;
    A[0][1] = -1;
    A[0][2] = 0;
    A[1][0] = -1;
    A[1][1] = 2;
    A[1][2] = -1;
    A[2][0] = 0;
    A[2][1] = -1;
    A[2][2] = 1;

    for(int i=0; i < 3; i++) {
        for(int j=0; j < 3; j++) {
            L[i][j] = 0;
        }
        D[i] = 0;
    }

    double b[3] = {1, 0, 0};

    dLDLt_solver(A, L, D, y, n, b, x, 0);

    EXPECT_NEAR(x[0], 1.0, 0.0001);
    EXPECT_NEAR(x[1], 1.0, 0.0001);
    EXPECT_NEAR(x[2], 1.0, 0.0001);
    EXPECT_NEAR(L[0][0], 1.0, 0.0001);
    EXPECT_NEAR(L[1][1], 1.0, 0.0001);
    EXPECT_NEAR(L[2][2], 1.0, 0.0001);
    EXPECT_NEAR(L[1][0], -0.5, 0.0001);
    EXPECT_NEAR(L[2][1], -0.66667, 0.0001);
    EXPECT_NEAR(D[0], 2.0, 0.0001);
    EXPECT_NEAR(D[1], 1.5, 0.0001);
    EXPECT_NEAR(D[2], 0.3333, 0.0001);

    for(i=0; i < n; i++) {
        free(A[i]);
        free(L[i]);
    }
    free(A);
    free(L);
}

TEST_F(LDLtFactorizationTest, ldlt_factorize)
{

    /* Calculate L and D for A
                      [  2  -3   4 ]
                [A] = [ -3   2  -3 ]
                      [  4  -3   1 ]
    */

    int i, n=3;

    double **L;
    double D[n];
    double **A;

    A = (double**)malloc(sizeof(double *) * n);
    L = (double**)malloc(sizeof(double *) * n);
    for(i=0; i < n; i++) {
        A[i] = (double *)malloc(sizeof(double)*n);
        L[i] = (double *)malloc(sizeof(double)*n);
    }

    A[0][0] = 2;
    A[0][1] = -3;
    A[0][2] = 4;
    A[1][0] = -3;
    A[1][1] = 2;
    A[1][2] = -3;
    A[2][0] = 4;
    A[2][1] = -3;
    A[2][2] = 1;

    for(int i=0; i < 3; i++) {
        for(int j=0; j < 3; j++) {
            L[i][j] = 0;
        }
        D[i] = 0;
    }

    dLDLt_solver(A, L, D, NULL, n, NULL, NULL, 1);

    EXPECT_NEAR(L[0][0], 1.0, 0.0001);
    EXPECT_NEAR(L[0][1], 0.0, 0.0001);
    EXPECT_NEAR(L[0][2], 0.0, 0.0001);
    EXPECT_NEAR(L[1][0], -1.5, 0.0001);
    EXPECT_NEAR(L[1][1], 1.0, 0.0001);
    EXPECT_NEAR(L[1][2], 0.0, 0.0001);
    EXPECT_NEAR(L[2][0], 2.0, 0.0001);
    EXPECT_NEAR(L[2][1], -1.2, 0.0001);
    EXPECT_NEAR(L[2][2], 1.0, 0.0001);

    EXPECT_NEAR(D[0], 2.0, 0.0001);
    EXPECT_NEAR(D[1], -2.5, 0.0001);
    EXPECT_NEAR(D[2], -3.4, 0.0001);

    for(i=0; i < n; i++) {
        free(A[i]);
        free(L[i]);
    }
    free(A);
    free(L);
}

TEST_F(LDLtFactorizationTest, ldlt_solve_knownLD)
{

    /* Solve  AX = b when L and D are already known */

    int i, n=3;

    double **L;
    double D[n];
    double x[n];
    double y[n];

    L = (double**)malloc(sizeof(double *) * n);
    for(i=0; i < n; i++) {
        L[i] = (double *)malloc(sizeof(double)*n);
    }

    L[0][0] = 1;
    L[0][1] = 0;
    L[0][2] = 0;
    L[1][0] = -0.5;
    L[1][1] = 1;
    L[1][2] = 0;
    L[2][0] = 0;
    L[2][1] = -0.66667;
    L[2][2] = 1;
    D[0] = 2.0;
    D[1] = 1.5;
    D[2] = 0.33333;

    double b[3] = {1, 0, 0};

    dLDLt_solver(NULL, L, D, y, n, b, x, 2);

    EXPECT_NEAR(x[0], 1.0, 0.0001);
    EXPECT_NEAR(x[1], 1.0, 0.0001);
    EXPECT_NEAR(x[2], 1.0, 0.0001);

    for(i=0; i < n; i++) {
        free(L[i]);
    }
    free(L);
}

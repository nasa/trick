/*
   PURPOSE: (Solve a linear set of equations: [A]x = b ; by first decomposing the SYMMETRIC, POSITIVE DEFINITE [A]
   matrix into a Lower Triangular, [L], form such that:

   [A] = [L][L_transpose] .

   The then backsubstitute to solve for x:

   [L]y = b ; [L_transpose]x = y .)

   REFERENCE: (((Cholesksy) (The cholesky book) (book) (Publisher) (Date) (Pages)))

   ASSUMPTIONS AND LIMITATIONS: (([A] is a square matrix) ([A] is symmetric) ([A] is positive definite) (Only the lower
   triangular of [A] needs to be loaded.))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (2/1/91) (Trick-CR-00000) (Initial Release))) */

#include <stdio.h>
#include "trick/trick_math.h"

#define TINY (1.0e-20)

int dLU_solver(double **A,      /* In: The input matrix [A] */
               double **L,      /* Out: Workspace for the Lower triangularization of [A] */
               double *y,       /* Out: Workspace for the intermediate 'y' vector */
               int n,           /* In: The square dimension of [A] */
               double *b,       /* In: The known vector 'b' */
               double *x,       /* Out: The output vector 'x' */
               int mode)
{                                      /* In: Solution mode: 0 = Solve [A]x = b for x 1 = Compute [L] Only 2 = Already
                                          have [L] from previous call, solve for x */

    int i, j, k;
    double sum, sum_jk, sum_ijk;
    double *Li;
    double *Lj;

    char buf[256];

    if (mode == 0 || mode == 1) {

        if (-TINY < A[0][0] && A[0][0] < TINY) {
            sprintf(buf, "ERROR: Diagonal Element A[0][0] = %.17g " "is too small...\nEXITING...\n", A[0][0]);
            return (TM_DIAG_SMALL);
        }
        L[0][0] = sqrt(A[0][0]);
        for (i = 0; i < n; i++)
            L[i][0] = A[i][0] / L[0][0];

        for (j = 1; j < n; j++) {
            if (-TINY < A[j][j] && A[j][j] < TINY) {
                sprintf(buf,
                        "ERROR: Diagonal Element " "A[%d][%d] = %.17g is too small...\n" "EXITING...\n", j, j, A[j][j]);
                return (TM_DIAG_SMALL);
            }

            Lj = L[j];
            sum_jk = 0.0;
            for (k = 0; k < j; k++)
                sum_jk += (Lj[k] * Lj[k]);
            Lj[j] = sqrt(A[j][j] - sum_jk);

            for (i = j + 1; i < n; i++) {
                Li = L[i];
                sum_ijk = 0.0;
                for (k = 0; k < j; k++)
                    sum_ijk += (Li[k] * Lj[k]);
                Li[j] = (A[i][j] - sum_ijk) / Lj[j];
            }
        }
    }

    if (mode == 0 || mode == 2) {

        y[0] = b[0] / L[0][0];
        for (i = 1; i < n; i++) {
            Li = L[i];
            sum = 0.0;
            for (j = 0; j < i; j++)
                sum += (Li[j] * y[j]);
            y[i] = (b[i] - sum) / Li[i];
        }

        x[n - 1] = y[n - 1] / L[n - 1][n - 1];
        for (i = n - 2; i >= 0; i--) {
            sum = 0.0;
            for (j = n - 1; j > i; j--)
                sum += (L[j][i] * x[j]);
            x[i] = (y[i] - sum) / L[i][i];
        }
    }

    return (TM_SUCCESS);
}

/*
   PURPOSE: (Solve a linear set of equations: [A]x = b ; by first decomposing the SYMMETRIC, POSITIVE
   SEMI-DEFINITE [A] matrix into a Lower Triangular [L], and Diagonal [D] such that:

   [A] = [L][D][L_transpose] .

   The then backsubstitute to solve for x:

   [L]y = b ; [D][L_transpose]x = y .)

   ASSUMPTIONS AND LIMITATIONS: (([A] is a square matrix) ([A] is symmetric) ([A] is positive semidefinite) (Only the lower
   triangular of [A] needs to be loaded.))

   PROGRAMMERS: (((N Arvind Kumar) (NASA JSC/CACI) (11/11/24) )) */

#include <stdio.h>

int dLDLt_solver(double **A,  /* In:  Input matrix [A] */
                 double **L,  /* Out: Lower triangular matrix in the factors LDLt */
                 double *D,   /* Out: Diagonal elements of the diagonal matrix D in LDLt */
                 double *y,   /* Out: Workspace for intermediate 'y' vector */
                 int n,       /* In:  Square dimension of [A] */
                 double *b,   /* In:  Known vector 'b' in the equation AX=b */
                 double *x,   /* Out: Output vector 'x' */
                 int mode)    /* In:  Solution mode:
                                        0 = Solve [A]x = b for x
                                        1 = Compute [L] and [D] only
                                        2 = Already have [L] and [D], solve for x */
{
    int i, j, k;
    double total;

    if(mode == 0 || mode == 1) {
        for (i = 0; i < n; i++) {
            L[i][i] = 1;
            D[i] = A[i][i];
            for (j = 0; j < i; j++) {
                D[i] -= L[i][j] * L[i][j] * D[j];
            }
            for (j = i + 1; j < n; j++) {
                L[j][i] = A[j][i];
                for (k = 0; k < i; k++) {
                    L[j][i] -= L[j][k] * L[i][k] * D[k];
                }
                L[j][i] /= D[i];
            }
        }
    }

    if(mode == 0 || mode == 2) {
        y[0] = b[0] / L[0][0];
        for(i=1; i < n; i++) {
            total = 0;
            for(j=0; j<i; j++) {
                total += L[i][j] * y[j];
            }
            y[i] = (b[i] - total) / L[i][i];
        }
    
        x[n-1] = y[n-1]/(D[n-1] * L[n-1][n-1]);
        for(i=n-2; i>=0; i--)
        {
            total = 0;
            for(j=i+1; j<n; j++)
            {
                total += D[i] * L[j][i] * x[j];
            }
            x[i] = (y[i]-total) / (D[i] * L[i][i]);
        }
    }
    return 0;
}

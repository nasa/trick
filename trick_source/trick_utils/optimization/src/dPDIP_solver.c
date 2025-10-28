/*
   PURPOSE: (Use the Primal-Dual Interior Point Method to optimize a linear programming problem subject to a set of equality constraints given by [A]x = b and inequality constraints of the form X1 > 0, X2 > 0, etc., for each variable in the problem.

   ASSUMPTIONS AND LIMITATIONS: ((The problem is in standard form with the inequality constraints being trivial inequalities, that is, each variable is greater than 0) (The problem is formulated as a minimization problem) ([A] is a square matrix) (The constant term in the objective function is not passed as an argument) (initial guess is in the feasible region))

   PROGRAMMERS: (((N Arvind Kumar) (NASA JSC/CACI) (10/01/25) )) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "trick/trick_math.h"

/***********************************************************************************************
 ***                                                                                           *
 *** @details  Implementation of Primal-Dual Interior point method for the linear case.        *
 **********************************************************************************************/
int dPDIP_linear_solver(int      num_variables,       /* total number of variables */
                        double*  objective_fn,        /* objective function coefficients */
                        int      num_equalities,      /* number of equality constraints */
                        double** eq_constraints,      /* equality constraints matrix (lhs) */
                        double*  constraints_rhs,     /* rhs of equality constraints */
                        double*  xvals,               /* in/out initial guess and solution */
                        double*  init_lambda,         /* inequalities lagrange multipliers */
                        double*  init_nu,             /* equalities lagrange multipliers */
                        double   epsilon,             /* desired precision */
                        double   alpha,               /* decrease in value per iteration
                                                          (typically between 0.01 and 0.3) */
                        double   beta,                /* backtracking step reduction factor
                                                          (typically between 0.1 and 0.8) */
                        double   mu,                  /* KKT condition perturbation factor */
                        double   mu_scaling_factor,   /* factor to change mu across iterations
                                                          (helps fine tune the algorithm) */
                        int      max_iterations,      /* max iterations of algorithm */
                        int      max_backtrack_cycles /* max cycles in backtracking search */
                       )
{
    int    i=0, j=0, k=0;
    double step_size = 1;
    double ratio = 0;
    double eta;
    double t;
    double loopsum = 0.0;
    double norm_rprim = 0.0;
    double norm_rcent = 0.0;
    double norm_rdual = 0.0;

    int    num_inequalities = num_variables;
    double x[num_variables];
    double deltax[num_variables];
    double lambda[num_inequalities];
    double nu[num_equalities];
    double updatedx[num_variables];
    double deltaLambda[num_inequalities];
    double updatedLambda[num_inequalities];
    double updatedNu[num_equalities];

    int    blockmatrix_size = num_variables + num_equalities;
    double **A;
    double B[blockmatrix_size];
    double soln[blockmatrix_size];
    double **L;
    double D[blockmatrix_size];
    double y[blockmatrix_size];
    int    num_iterations = 0;

    A = (double**)malloc(sizeof(double *)*blockmatrix_size);
    L = (double**)malloc(sizeof(double *)*blockmatrix_size);
    for(i=0; i < blockmatrix_size; i++) {
        A[i] = (double*)malloc(sizeof(double)*blockmatrix_size);
        L[i] = (double*)malloc(sizeof(double)*blockmatrix_size);
        for(j=0; j < blockmatrix_size; j++) {
            A[i][j] = 0;
            L[i][j] = 0;
        }
        B[i] = 0;
        D[i] = 0;
        y[i] = 0;
        soln[i] = 0;
    }

    for(i=0; i<num_variables; i++) {
        x[i] = xvals[i];
    }
    for(i=0; i<num_inequalities; i++) {
        lambda[i] = init_lambda[i];
    }
    for(i=0; i<num_equalities; i++) {
        for(j=0; j < num_variables; j++) {
            A[i+num_variables][j] = eq_constraints[i][j];
            A[j][i+num_variables] = eq_constraints[i][j];
        }
        nu[i] = init_nu[i];
    }

    while(1) {
        num_iterations++;

        eta = 0.0;
        for(i=0; i<num_inequalities; i++) {
            eta += x[i] * lambda[i];
        }
        mu = mu * mu_scaling_factor;
        t = mu * num_inequalities / eta;

        for(i=0; i < num_variables; i++) {
            A[i][i] = lambda[i] / x[i];
            loopsum = 0.0;
            for(j=0; j < num_equalities; j++) {
                loopsum += eq_constraints[j][i] * nu[j];
            }
            B[i] = -(objective_fn[i] + loopsum - 1/(t * x[i]));
        }
        for(i=0; i < num_equalities; i++) {
            loopsum = 0.0;
            for(j=0; j < num_variables; j++) {
                loopsum += eq_constraints[i][j] * x[j];
            }
            B[i+num_variables] = constraints_rhs[i] - loopsum;
        }

        dLDLt_solver(A, L, D, y, blockmatrix_size, B, soln, 0);

        double norm_b_x = 0.0;
        for(i=0; i < num_variables; i++) {
            deltax[i] = soln[i];
            loopsum = 0.0;
            for(j=0; j < num_equalities; j++) {
                loopsum += eq_constraints[j][i] * nu[j];
            }
            norm_b_x += (objective_fn[i] - lambda[i] + loopsum) * (objective_fn[i] - lambda[i] + loopsum);
        }
        for(i=num_variables; i < num_variables+num_equalities; i++) {
            norm_b_x += B[i] * B[i];
        }
        for(i=0; i < num_inequalities; i++) {
            norm_b_x += (lambda[i] - 1/t) * (lambda[i] - 1/t);
        }

        /* backtracking line search */

        for(i=0; i<num_inequalities; i++) {
            deltaLambda[i] =  1/(xvals[i] * t) - lambda[i] - lambda[i]/xvals[i] * deltax[i];
        }
        step_size = 1;
        for(i=0; i < num_inequalities; i++) {
            if(deltaLambda[i] < 0) {
                ratio = -lambda[i]/deltaLambda[i];
                if(ratio < step_size)
                    step_size = ratio;
            }
        }
        step_size = 0.999 * step_size;
        for(i=0; i < num_variables; i++) {
            updatedx[i] = x[i] + step_size * deltax[i];
        }

        for(i=0; i < num_inequalities; i++) {
            int count = 0;

            while(updatedx[i] <= -1e-5)
            {
                if(count++ > 25)
                    break;
                step_size = beta*step_size;
                for(j=0; j < num_variables; j++) {
                    updatedx[j] = x[j] + step_size * deltax[j];
                } 
            }
        }
        int backtrack_cycle = 0;
        while(1) {
            backtrack_cycle++;
            step_size = beta * step_size;
            for(i=0; i < num_variables; i++) {
                updatedx[i] = x[i] + step_size * deltax[i];
            }
            for(i=0; i<num_inequalities; i++) {
                updatedLambda[i] = lambda[i] + step_size * deltaLambda[i];
            }
            for(i=0; i<num_equalities; i++) {
                updatedNu[i] = nu[i] + step_size * soln[i+num_variables];
            }
            t = mu * num_inequalities / eta;
            norm_rprim = 0.0;
            norm_rcent = 0.0;
            norm_rdual = 0.0;

            for(i=0; i < num_variables; i++) {
                loopsum = 0.0;
                for(j=0; j < num_equalities; j++) {
                    loopsum += eq_constraints[j][i] * updatedNu[j];
                }
                B[i] = -(objective_fn[i] + loopsum - 1/(t * updatedx[i]));
                norm_rdual += (objective_fn[i] - updatedLambda[i] + loopsum) * (objective_fn[i] - updatedLambda[i] + loopsum);
            }

            for(i=0; i < num_equalities; i++) {
                loopsum = 0.0;
                for(j=0; j < num_variables; j++) {
                    loopsum += eq_constraints[i][j] * updatedx[j];
                }
                B[i+num_variables] = constraints_rhs[i] - loopsum;
            }

            double norm_b_updatedx = 0.0;
            for(i=num_variables; i < num_variables+num_equalities; i++) {
                norm_rprim += B[i] * B[i];
            }
            for(i=0; i < num_inequalities; i++) {
                norm_rcent += (lambda[i] - 1/t) * (lambda[i] - 1/t);
            }
            norm_b_updatedx = norm_rcent + norm_rprim + norm_rdual;
            if(norm_b_updatedx < (1-alpha*step_size)*norm_b_x)
                break;
            double previous_value = 0.0;
            double updated_value = 0.0;
            double delta_value = 0.0;
            for(i=0; i < num_variables; i++) {
                previous_value += objective_fn[i] * x[i];
                delta_value += objective_fn[i] * deltax[i];
                updated_value += objective_fn[i] * updatedx[i];
            }
            if(updated_value <= previous_value + alpha * step_size * delta_value)
                break;
            if(backtrack_cycle >= max_backtrack_cycles)
                break;
        }

        for(i=0; i < num_variables; i++) {
            x[i] = updatedx[i];
        }
        eta = 0.0;
        for(i=0; i < num_inequalities; i++) {
            lambda[i] = updatedLambda[i];
            eta += x[i] * lambda[i];
        }
        for(i=0; i < num_equalities; i++) {
            nu[i] = updatedNu[i];
        }
        for(i=0; i < num_variables; i++) {
            xvals[i] = x[i];
        }
        if(eta < epsilon && sqrt(norm_rprim + norm_rdual) < epsilon)
            break;
        if(num_iterations >= max_iterations)
            break;
    }
    for(i=0; i < blockmatrix_size; i++) {
        free(A[i]);
        free(L[i]);
    }
    free(A);
    free(L);
    return num_iterations;
}

/******************************************************************************************
 * Function: mark_nonzeros                                                                *
 * Purpose:  (Mark non-zero rows and columns. Helper function for reduce_and_solve.)      *
 * ***************************************************************************************/

void mark_nonzeros(int r,
                   int c,
                   double **A,
                   int *rflags,
                   int *cflags,
                   int *nzrows,
                   int *nzcols)
{
    const double EPSILON = 1e-15;
    for (int i=0; i < r; i++)
    {
        rflags[i] = 0;
    }
    for (int i=0; i < c; i++)
    {
        cflags[i] = 0;
    }
    for (int i=0; i < r; i++)
    {
        for (int j=0; j < c; j++)
        {
            if(A[i][j] > EPSILON || A[i][j] < -EPSILON)
            {
                if(rflags[i] != 1) {
                    rflags[i] = 1;
                    (*nzrows)++;
                }
                if(cflags[j] != 1) {
                    cflags[j] = 1;
                    (*nzcols)++;
                }
            }
        }
    }
}

/******************************************************************************************
 * Function: reduce_and_solve                                                             *
 * Purpose:  (Remove zero rows and zero columns along with corresponding unknowns and rhs *
 * values and solve the resulting equation.)                                              *
 * ***************************************************************************************/

int reduce_and_solve(int num_vars,
                     double *minfn,
                     int num_eq,
                     double **Amat,
                     double *B,
                     double *X,
                     double *lambdas,
                     double *nus,
                     double epsilon,
                     double alpha,
                     double beta,
                     double mu,
                     double mu_scaler,
                     int maxiter,
                     int maxbacktrack)
{
    int count = 0;
    int rflags[num_eq], cflags[num_vars], nzrows=0, nzcols=0;
    mark_nonzeros(num_eq, num_vars, Amat, rflags, cflags, &nzrows, &nzcols);
    double reducedminfn[nzcols];
    double reducedlambdas[nzcols];
    double reducednus[nzrows];

    double **Cmat = (double**)malloc(sizeof(double *) * nzrows);
    for(int i=0; i < nzrows; i++) {
        Cmat[i] = (double *)malloc(sizeof(double) * nzcols);
        for(int j=0; j<nzcols; j++) {
            Cmat[i][j] = 0.0;
        }
    }

    int Crow=0;
    for(int j=0; j < num_eq; j++)
    {
        int Ccol = 0;
        if(rflags[j] == 1)
        {
            for(int k=0; k < num_vars; k++)
            {
                if(cflags[k] == 1)
                {
                    Cmat[Crow][Ccol++] = Amat[j][k];
                }
            }
            Crow++;
        }
    }

    double yvals[nzcols];
    for(int i=0, j=0; j < num_vars; j++)
    {
        if(cflags[j] == 1)
        {
            yvals[i] = X[j];
            reducedminfn[i] = minfn[j];
            reducedlambdas[i] = lambdas[j];
            i++;
        }
    }

    double dvals[nzrows];
    for(int i=0, j=0; j < num_eq; j++)
    {
        if(rflags[j] == 1)
        {
            dvals[i] = B[j];
            reducednus[i] = nus[j];
            i++;
        }
    }
    count = dPDIP_linear_solver(nzcols, reducedminfn, nzrows, Cmat, dvals, yvals,
                                reducedlambdas, reducednus, epsilon,
                                alpha, beta, mu, mu_scaler, maxiter, maxbacktrack);
    for(int i=0, j=0; i < num_vars; i++)
    {
        if(cflags[i] == 1)
        {
            X[i] = yvals[j++];
        } else {
            X[i] = 0;
        }
    }

    for(int i=0; i < nzrows; ++i) {
        free(Cmat[i]);
    }
    free(Cmat);
    return count;
}

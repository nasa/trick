# Primal Dual Interior Point Algorithm

## Contents

* [Introduction](#Introduction)
* [Algorithm Interface](#Algorithm-Interface)
* [Arguments](#Arguments)
* [Example](#Example)
* [Tutorial](#Tutorial)

<a id=Introduction></a>
## Introduction
The Primal-Dual Interior Point algorithm is a fast interior point optimization algorithm that has been implemented for the linear case. It can be run as part of trick or can be copied over and run in the standalone mode. To compile and run as a standalone program, copy the files trick_utils/optimization/src/dPDIP_solver.c and trick_utils/math/src/dLDLt_solver.c.

<a id=Algorithm-Interface></a>
## Algorithm Interface
The algorithm interface is dPDIP_linear_solver which solves for the values of xvals such that the objective function is minimized subject to the given equality constraints. Alternatively, the wrapper function reduce_and_solve can be invoked. The wrapper function strips the rows and columns that contain only 0s along with the corresponding variables representing the unknown values that are to be calculated and/or values on the right side of the equation before invoking dPDIP_linear_solver. After finding a solution, the wrapper returns the soluton with 0s for any variables that had been stripped.

### Description
A typical optimization problem seeks to minimize an objective function subject to certain constraints. Since a maximization problem can be represented as a minimization problem with the signs of the variables reversed and the inequality constraints can be converted to equality constraints by adding slack variables, the standard form of the problem seeks to minimize a function subject to equality constraints. Further, the standard form also assumes that all variables are positive since every problem can be transformed to a form in which all variables are positive.
  
The following C function which solves a linear optimization problem in its standard form is provided. That is, it minimizes an objective function subject to the given equality constraints and all variables being positive.

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

The function returns the total number of iterations.

<a id=Arguments></a>
## Arguments

For a problem which requires minimizing the function f(X) subject to AX = b, where X is a vector of unknowns, A is a matrix containing the coefficients of the equality constraints, and b is a column vector containing the right hand side of the equality constraints, f is represented by objective_fn in the interface, A is represented by eq_constraints, X by xvals, and b by constraints_rhs.

Intial values representing an initial must be filled into xvals, init_lambda and init_nu. Here, init_lambda and init_nu are the lagrange multipliers (although eq_constraints represents both the equality constraints and the inequalities after adding the slack variables, the corresponding lagrange multipliers are listed separately).

num_variables and num_equalities represent the total number of variables and the total number of equalities respectively. The value of the desired precision is represented by epsilon.

The values of alpha, beta, mu and mu_scaling_factor are used by the algorithm. While alpha controls the decrease in value per iteration, beta controls the backtracking step reduction. Typical values for alpha are between 0.01 and 03 while typical values for beta are between 0.1 and 0.8. The value of mu is used to control the perturbation factor while the value of mu_scaling_factor is multiplied to mu during every iteration and can be used to change mu across iterations.

Finally, max_iterations and max_backtrack_cycles are the maximum values of the number of times the main loop of the algorithm and the loop in the backtracking step of the algorithm evaluate before breaking out of the respective loops.

<a id=Example></a>
## Example

example()
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
        subject to the constraints
        9.5X1 + 4X2 + X3 = 97.5
        X1 + X4 = 9
        X1 + X2 - X5 = 2
        X1 - 2X2 - X6 = -20
        3X1 + 15X2 - X7 = 15
        X1 >= 0, X2 >= 0, X3 >= 0, X4 >= 0, X5 >= 0, X6 >= 0, X7 >= 0
        We now have 7 variables and 5 equalities with all variables being positive.
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

    /* solution set is now in xvals. */
}

<a id=Tutorial></a>
## Tutorial

[Tutorial](Optimization.pdf)


#ifndef TRICK_OPTIMIZATION_H
#define TRICK_OPTIMIZATION_H

/*
 * Function prototypes for all functions in ${TRICK_HOME}/sim_services/optimization
 */

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup TRICK_OPTIMIZATION Trick Optimization Utilities
    $TRICK_HOME/trick_source/trick_utils/optimization/include/trick_optimization.h
 */

/**
* @ingroup TRICK_OPTIMIZATION
* Primal-Dual Interior Point Method for linear program.
*/
int dPDIP_linear_solver(int num_variables, double* objective_fn, int num_equalities,
                       double** eq_constraints, double* constraints_rhs, double* xvals,
                       double* init_lambda, double* init_nu, double epsilon, double alpha,
                       double beta, double mu, double mu_scaling_factor,
                       int max_iterations, int max_backtrack_cycles);

#ifdef __cplusplus
}
#endif

#endif

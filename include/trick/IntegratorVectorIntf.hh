#ifndef INTEGRATOR_VECTOR_INTF_HH
#define INTEGRATOR_VECTOR_INTF_HH
/**
PURPOSE: (std::vector interface for Integrator that leverages existing C-interface functions)
 */

#include <vector>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Load simulation state variables into the integrator from a vector of pointers.
 * Equivalent to load_state(&s0, &s1, ..., NULL).
 * @param states  Vector of pointers to state variables. Size must equal num_state.
 */
void load_state_vec(const std::vector<double*>& states);

/**
 * Load first derivatives into the integrator from a vector of pointers.
 * Equivalent to load_deriv(&d0, &d1, ..., NULL).
 * @param derivs  Vector of pointers to derivative variables. Size must equal num_state.
 */
void load_deriv_vec(const std::vector<double*>& derivs);

/**
 * Load second derivatives into the integrator from a vector of pointers.
 * Equivalent to load_deriv2(&d0, &d1, ..., NULL).
 * @param derivs2  Vector of pointers to second derivative variables. Size must equal num_state.
 */
void load_deriv2_vec(const std::vector<double*>& derivs2);

/**
 * Unload integrated state variables from the integrator into a vector of pointers.
 * Equivalent to unload_state(&s0, &s1, ..., NULL).
 * @param states  Vector of pointers to state variables to be updated. Size must equal num_state.
 */
void unload_state_vec(const std::vector<double*>& states);

#ifdef __cplusplus
}
#endif

#endif /* INTEGRATOR_VEC_INTF_HH */

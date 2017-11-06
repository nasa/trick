#ifndef INTEGRATOR_C_INTF_H
#define INTEGRATOR_C_INTF_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int integrate(void);
int integrate_1st_order_ode(const double* deriv, double* state);
int integrate_2nd_order_ode(const double* acc, double* vel, double * pos);
double get_integ_time(void);
void set_integ_time(double time_value);
void load_state(double* arg1, ... );
void load_deriv( double* arg1, ...);
void load_deriv2( double* arg1, ...);
void unload_state (double* arg1, ...);
void load_indexed_state( unsigned int index, double state );
void load_indexed_deriv( unsigned int index, double deriv );
void load_indexed_deriv2(unsigned int index, double deriv2);
double unload_indexed_state( unsigned int index );
int  get_intermediate_step(void);
void set_intermediate_step(int intermediate_step_value);
int get_integ_type(void);

#ifdef __cplusplus
}
#endif
#endif


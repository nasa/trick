#ifndef INTEGRATOR_HH
#define INTEGRATOR_HH
/**
 PURPOSE: (Integrator Base Class)
 */
#if defined(TRICK_VER) && !defined(TEST)
    #include "trick/memorymanager_c_intf.h"
    #define INTEG_NEW(class) (class*)TMM_declare_var_1d(#class,1)
    #define INTEG_ALLOC(typespec, num) (typespec*)TMM_declare_var_1d(#typespec,(num))
    #define INTEG_FREE(p) TMM_delete_var_a(p)
#else
    #include <stdlib.h>
    #define INTEG_NEW(class) new class
    #define INTEG_ALLOC(typespec, num) (typespec*)calloc((size_t)num,sizeof(typespec))
    #define INTEG_FREE(p) free(p)
#endif
#include <cstdarg>
/*
#ifdef USE_ER7_UTILS_INTEGRATORS
#include "er7_utils/integration/core/include/integration_technique.hh"
#endif
*/

/**
 *  Below are the supported types of integration schemes.
 */
typedef enum {
    Euler                   = 0, /* 1st Order Explicit Euler */
    Euler_Cromer            = 1, /* The simple symplectic Euler-Cromer */
    Nystrom_Lear_2          = 2, /* 2nd Order Nystrom Lear */
    Runge_Kutta_2           = 3, /* 2nd Order Runge Kutta */
    Modified_Midpoint_4     = 4, /* 4th Order Modified Midpoint */
    Runge_Kutta_4           = 5, /* 4th Order Runge Kutta */
    Runge_Kutta_Gill_4      = 6, /* 4th Order Runge Kutta Gill */
    Runge_Kutta_Fehlberg_45 = 7, /* 5th Order Runge Kutta Fehlberg */
    Runge_Kutta_Fehlberg_78 = 8, /* 8th Order Runge Kutta Fehlberg */
    ABM_Method              = 9, /* Adams-Bashforth-Moulton Method */
    User_Defined            = 10 /* User defined integrator */
} Integrator_type;

namespace Trick {
/**
 *  Contains the information shared by all of the Integrator Schemes.
 *
 * @author John Penn
 * @author Alex Lin
 * @author Donna Panter
 *
 * @date May 2010
 */
    class Integrator {

    public:

        Integrator();
        virtual ~Integrator() {};

        virtual void initialize(int State_size, double Dt) = 0;
        virtual int integrate() = 0;

        virtual int integrate_1st_order_ode (
           double const* derivs_in, double* state_in_out);
        virtual int integrate_2nd_order_ode (
           double const* accel, double* velocity, double* position);

#ifndef SWIGPYTHON
        void state_reset ();
        void state_element_in (unsigned int index , double* state_p);
#endif

#ifndef SWIGPYTHON
        void state_in (double* arg1, va_list argp);
#endif
        void state_in (double* arg1, ...)
#ifndef SWIGPYTHON
        #ifdef __GNUC__
          #if __GNUC__ >= 4
          __attribute__((sentinel))
          #endif
        #endif
#endif
        ;
#ifndef SWIGPYTHON
        void deriv_in (double* arg1, va_list argp);
#endif
        void deriv_in (double* arg1, ...)
#ifndef SWIGPYTHON
        #ifdef __GNUC__
          #if __GNUC__ >= 4
          __attribute__((sentinel))
          #endif
        #endif
#endif
        ;
#ifndef SWIGPYTHON
        void state_out (double* arg1, va_list argp);
#endif
        void state_out(double* arg1, ...)
#ifndef SWIGPYTHON
        #ifdef __GNUC__
          #if __GNUC__ >= 4
          __attribute__((sentinel))
          #endif
        #endif
#endif
        ;

#ifndef SWIGPYTHON
        void deriv2_in (double* arg1, va_list argp);
#endif
        void deriv2_in (double* arg1, ...)
#ifndef SWIGPYTHON
        #ifdef __GNUC__
          #if __GNUC__ >= 4
          __attribute__((sentinel))
          #endif
        #endif
#endif
        ;

        int num_state;

        int intermediate_step;

        bool first_step_deriv;
        bool last_step_deriv;
        bool is_2nd_order_ODE_technique;  // -- set by integration technique
        bool use_deriv2;                  // -- set by integration technique

        double dt;               // -- set by IntegLoopSimObject.cpp
#ifndef USE_ER7_UTILS_INTEGRATORS
        double **state_origin;
#endif
        double *state;
        double **deriv;
        double **deriv2;
        double **state_ws;

        double time;
        double time_0;

        int verbosity;

        virtual bool get_first_step_deriv() ;
        virtual void set_first_step_deriv(bool first_step) ;
        virtual bool get_last_step_deriv() ;
        virtual void set_last_step_deriv(bool last_step) ;
        virtual void set_verbosity(int level);
        virtual void reset() {}
        virtual Integrator_type get_Integrator_type() { return (User_Defined); };

    };

    Integrator* getIntegrator( Integrator_type Alg, unsigned int State_size, double Dt = 0.0 );

}

#endif

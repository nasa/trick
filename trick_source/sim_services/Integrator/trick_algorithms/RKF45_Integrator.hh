#ifndef RKF45_INTEGRATOR_HH
#define RKF45_INTEGRATOR_HH
#include "../include/Integrator.hh"
/**
 PURPOSE: (Integrator using Runge Kutta Fehlberg 45 method.)
 */

namespace Trick {

    /** Integrator using Runge Kutta Fehlberg 45 method.
     */
    class RKF45_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        RKF45_Integrator() {};
        RKF45_Integrator( int State_size, double Dt);
        virtual ~RKF45_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        void set_first_step_deriv(bool first_step);

        Integrator_type get_Integrator_type() { return(Runge_Kutta_Fehlberg_45); } ;    
    };
}
#endif

#ifndef _RK4_INTEGRATOR_HH_
#define _RK4_INTEGRATOR_HH_
#include "../include/Integrator.hh"
/**
 PURPOSE: (Integrator using Runga Kutta 4 method.)
 */

namespace Trick {

    /** Integrator using Runge Kutta 4 method.
     */
    class RK4_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        RK4_Integrator() {};
        RK4_Integrator( int State_size, double Dt);
        ~RK4_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(Runge_Kutta_4); } ;
    };
}
#endif

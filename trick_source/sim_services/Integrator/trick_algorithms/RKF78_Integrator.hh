#ifndef RKF78_INTEGRATOR_HH
#define RKF78_INTEGRATOR_HH
#include "../include/Integrator.hh"
/**
 PURPOSE: (Integrator using Runga Kutta Fehlberg 78 method.)
 */

namespace Trick {

    /** Integrator using Runge Kutta Fehlberg 78 method.
     */
    class RKF78_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        RKF78_Integrator() {};
        RKF78_Integrator( int State_size, double Dt);
        virtual ~RKF78_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(Runge_Kutta_Fehlberg_78); } ;

    };
}
#endif

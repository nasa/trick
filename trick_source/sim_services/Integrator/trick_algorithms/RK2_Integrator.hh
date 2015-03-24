/*
PURPOSE:
    (Integrator using Runga Kutta 2 method.)
*/
#ifndef RK2_INTEGRATOR_HH
#define RK2_INTEGRATOR_HH
#include "../include/Integrator.hh"
namespace Trick {

    /** Integrator using Runga Kutta 2 method.
     */
    class RK2_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        RK2_Integrator() {};
        RK2_Integrator( int State_size, double Dt);
        virtual ~RK2_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(Runge_Kutta_2); } ;    
    };
}
#endif

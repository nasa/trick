#ifndef RKG4_INTEGRATOR_HH
#define RKG4_INTEGRATOR_HH
#include "../include/Integrator.hh"
/**
 PURPOSE: (Integrator using Runga Kutta Gill 4 method.)
 */
namespace Trick {

     /** Integrator using Runge Kutta Gill 4th Order method.
      */
    class RKG4_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        RKG4_Integrator() {};
        RKG4_Integrator( int State_size, double Dt);
        virtual ~RKG4_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(Runge_Kutta_Gill_4); } ;    
    };
}
#endif

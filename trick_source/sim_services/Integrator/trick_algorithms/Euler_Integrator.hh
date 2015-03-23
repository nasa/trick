#ifndef EULER_INTEGRATOR_HH
#define EULER_INTEGRATOR_HH
#include "../include/Integrator.hh"
/**
 PURPOSE: (Integrator using Euler method.)
 */
namespace Trick {

    /**  Integrator using Euler method.
     */
    class Euler_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        Euler_Integrator() {};
        Euler_Integrator( int State_size, double Dt);
        ~Euler_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        void set_first_step_deriv(bool first_step);
 
        Integrator_type get_Integrator_type() { return(Euler); } ;
    };
}
#endif

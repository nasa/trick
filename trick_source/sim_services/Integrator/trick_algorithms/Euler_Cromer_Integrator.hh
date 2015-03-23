#ifndef EULER_CROMER_INTEGRATOR_HH
#define EULER_CROMER_INTEGRATOR_HH
#include "../include/Integrator.hh"
/**
 PURPOSE: (Integrator using Euler Cromer method.)
 */
namespace Trick {

    /** Integrator using Euler Cromer method.
     */
    class Euler_Cromer_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        Euler_Cromer_Integrator() {};
        Euler_Cromer_Integrator( int State_size, double Dt);
        virtual ~Euler_Cromer_Integrator();

        void deriv2_in(double* arg1, ...)
#ifndef SWIGPYTHON
        #ifdef __GNUC__
          #if __GNUC__ >= 4
          __attribute__((sentinel))
          #endif
        #endif
#endif
        ;
        void initialize( int State_size, double Dt);
        int integrate();
        Integrator_type get_Integrator_type() { return(Euler_Cromer); } ;
    };
}
#endif

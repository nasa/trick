#ifndef _MM4_INTEGRATOR_HH_
#define _MM4_INTEGRATOR_HH_
#include "../include/Integrator.hh"
/**
 PURPOSE: (Integrator using Modified Midpoint method.)
 */

namespace Trick {

    class MM4_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        MM4_Integrator() {};
        MM4_Integrator( int State_size, double Dt);
        virtual ~MM4_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(Modified_Midpoint_4); } ;
    };
}
#endif

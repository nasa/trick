#ifndef _NL2_INTEGRATOR_HH_
#define _NL2_INTEGRATOR_HH_
#include "../include/Integrator.hh"
/**
 PURPOSE: (Integrator using Nystrom Lear 2 method.)
 */

namespace Trick {

    /** Integrator using Nystrom Lear 2 method.
     */
    class NL2_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        NL2_Integrator() {};
        NL2_Integrator( int State_size, double Dt);
        virtual ~NL2_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        void set_first_step_deriv(bool first_step);

        Integrator_type get_Integrator_type() { return(Nystrom_Lear_2); } ;    
    };
}
#endif

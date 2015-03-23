#ifndef ABM_INTEGRATOR_HH
#define ABM_INTEGRATOR_HH
#include "../include/Integrator.hh"
/**
 PURPOSE: (Integrator using ABM method.)
 */
namespace Trick {

    /** Integrator using ABM method.
     */
    class ABM_Integrator : public Integrator {

    public:
        /** Default Constructor. This must remain public so the MM can create these. */
        ABM_Integrator() {};
        ABM_Integrator( int State_size, double Dt);
        virtual ~ABM_Integrator();

        void initialize( int State_size, double Dt);

        int integrate();

        Integrator_type get_Integrator_type() { return(ABM_Method); };
    
        double time;
        double time_0;

        int counter;
        int primed;
        double **stored_data;

    };
}
#endif

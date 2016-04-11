/*******************************************************************************

Purpose:
  (Define class ABM_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef ABM_INTEGRATOR_HH
#define ABM_INTEGRATOR_HH

#ifdef USE_ER7_UTILS_INTEGRATORS
#include "er7_utils/integration/abm4/include/abm4_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/first_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for ABM_Integrator
 */
class ABM_IntegratorHelper {
protected:

   er7_utils::ABM4IntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with ABM integration. */
};


/**
 * Integrator using fourth order Adams Bashforth Moulton method.
 */
class ABM_Integrator :
   protected ABM_IntegratorHelper,
   public er7_utils::TrickFirstOrderOdeIntegrator {

public:

   /** Default constructor, needed by the MemoryManager. */
   ABM_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   ABM_Integrator (const ABM_Integrator & src)
   :
      ABM_IntegratorHelper (src),
      TrickFirstOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   ABM_Integrator (int state_size, double delta_t)
   :
      ABM_IntegratorHelper (),
      TrickFirstOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   ABM_Integrator & operator= (ABM_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickFirstOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~ABM_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return ABM_Method; }
};

}
#else
#include "trick/Integrator.hh"
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

        int counter;
        int primed;
        double **stored_data;

    };
}
#endif

#endif

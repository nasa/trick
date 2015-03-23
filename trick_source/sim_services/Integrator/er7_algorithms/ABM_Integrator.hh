/*******************************************************************************

Purpose:
  (Define class ABM_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef ABM_INTEGRATOR_HH
#define ABM_INTEGRATOR_HH

#include "er7_utils/integration/abm4/include/abm4_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/first_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for ABM_Integrator
 */
class ABM_IntegratorHelper {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__ABM_IntegratorHelper();
#endif

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
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__ABM_Integrator();
#endif

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

#endif

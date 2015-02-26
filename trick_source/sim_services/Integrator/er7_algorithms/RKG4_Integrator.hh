/*******************************************************************************

Purpose:
  (Define class RKG4_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef TRICK_ER7_RKG4_INTEGRATOR_HH
#define TRICK_ER7_RKG4_INTEGRATOR_HH

#include "er7_utils/integration/rkg4/include/rkg4_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/first_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for RKG4_Integrator
 */
class RKG4_IntegratorHelper {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__RKG4_IntegratorHelper();
#endif

protected:

   er7_utils::RKGill4IntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with RKG4 integration. */
};


/**
 * Integrator using Runge Kutta Gill 4 method.
 */
class RKG4_Integrator :
   protected RKG4_IntegratorHelper,
   public er7_utils::TrickFirstOrderOdeIntegrator {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__RKG4_Integrator();
#endif

public:

   /** Default constructor, needed by the MemoryManager. */
   RKG4_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   RKG4_Integrator (const RKG4_Integrator & src)
   :
      RKG4_IntegratorHelper (src),
      TrickFirstOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   RKG4_Integrator (int state_size, double delta_t)
   :
      RKG4_IntegratorHelper (),
      TrickFirstOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   RKG4_Integrator & operator= (RKG4_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickFirstOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~RKG4_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return Runge_Kutta_Gill_4; }
};

}

#endif

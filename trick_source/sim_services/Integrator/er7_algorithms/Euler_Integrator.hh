/*******************************************************************************

Purpose:
  (Define class Euler_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef TRICK_ER7_EULER_INTEGRATOR_HH
#define TRICK_ER7_EULER_INTEGRATOR_HH

#include "er7_utils/integration/euler/include/euler_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/first_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for Euler_Integrator
 */
class Euler_IntegratorHelper {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__Euler_IntegratorHelper();
#endif

protected:

   er7_utils::EulerIntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with Euler integration. */
};


/**
 * Integrator using Euler integration.
 */
class Euler_Integrator :
   protected Euler_IntegratorHelper,
   public er7_utils::TrickFirstOrderOdeIntegrator {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__Euler_Integrator();
#endif

public:

   /** Default constructor, needed by the MemoryManager. */
   Euler_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   Euler_Integrator (const Euler_Integrator & src)
   :
      Euler_IntegratorHelper (src),
      TrickFirstOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   Euler_Integrator (int state_size, double delta_t)
   :
      Euler_IntegratorHelper (),
      TrickFirstOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   Euler_Integrator & operator= (Euler_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickFirstOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~Euler_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return Euler; }
};

}

#endif

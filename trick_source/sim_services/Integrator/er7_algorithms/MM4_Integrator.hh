/*******************************************************************************

Purpose:
  (Define class MM4_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef TRICK_ER7_MM4_INTEGRATOR_HH
#define TRICK_ER7_MM4_INTEGRATOR_HH

#include "er7_utils/integration/mm4/include/mm4_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/second_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for MM4_Integrator
 */
class MM4_IntegratorHelper {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__MM4_IntegratorHelper();
#endif

protected:

   er7_utils::MM4IntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with MM4 integration. */
};


/**
 * Integrator using a modified midpoint approach.
 */
class MM4_Integrator :
   protected MM4_IntegratorHelper,
   public er7_utils::TrickSecondOrderOdeIntegrator {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__MM4_Integrator();
#endif

public:

   /** Default constructor, needed by the MemoryManager. */
   MM4_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   MM4_Integrator (const MM4_Integrator & src)
   :
      MM4_IntegratorHelper (src),
      TrickSecondOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   MM4_Integrator (int state_size, double delta_t)
   :
      MM4_IntegratorHelper (),
      TrickSecondOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   MM4_Integrator & operator= (MM4_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickSecondOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~MM4_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return Modified_Midpoint_4; }
};

}

#endif

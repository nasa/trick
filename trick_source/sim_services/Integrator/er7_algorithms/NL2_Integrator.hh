/*******************************************************************************

Purpose:
  (Define class NL2_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef TRICK_ER7_NL2_INTEGRATOR_HH
#define TRICK_ER7_NL2_INTEGRATOR_HH

#include "er7_utils/integration/nl2/include/nl2_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/second_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for NL2_Integrator
 */
class NL2_IntegratorHelper {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__NL2_IntegratorHelper();
#endif

protected:

   er7_utils::NystromLear2IntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with NL2 integration. */
};


/**
 * Integrator using second order Nystrom-Lear.
 */
class NL2_Integrator :
   protected NL2_IntegratorHelper,
   public er7_utils::TrickSecondOrderOdeIntegrator {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__NL2_Integrator();
#endif

public:

   /** Default constructor, needed by the MemoryManager. */
   NL2_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   NL2_Integrator (const NL2_Integrator & src)
   :
      NL2_IntegratorHelper (src),
      TrickSecondOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   NL2_Integrator (int state_size, double delta_t)
   :
      NL2_IntegratorHelper (),
      TrickSecondOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   NL2_Integrator & operator= (NL2_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickSecondOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~NL2_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return Nystrom_Lear_2; }
};

}

#endif

/*******************************************************************************

Purpose:
  (Define class RKF45_Integrator.)

Programmers:
  (((David Hammen) (Odyssey) (July 2012) (JEOD_2.2 #462)
    (Joint /JEOD integration framework)))

*******************************************************************************/

#ifndef RKF45_INTEGRATOR_HH
#define RKF45_INTEGRATOR_HH

#include "er7_utils/integration/rkf45/include/rkf45_integrator_constructor.hh"
#include "er7_utils/trick/integration/include/first_order_ode_integrator.hh"


namespace Trick {

/**
 * Helper class for RKF45_Integrator
 */
class RKF45_IntegratorHelper {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__RKF45_IntegratorHelper();
#endif

protected:

   er7_utils::RKFehlberg45IntegratorConstructor
   helper_integ_constructor; /* --
      The integrator constructor associated with RKF45 integration. */
};


/**
 * Integrator using Runge Kutta Fehlberg 4/5 method.
 */
class RKF45_Integrator :
   protected RKF45_IntegratorHelper,
   public er7_utils::TrickFirstOrderOdeIntegrator {
#ifndef SWIG
   friend class InputProcessor;
   friend void init_attrTrick__RKF45_Integrator();
#endif

public:

   /** Default constructor, needed by the MemoryManager. */
   RKF45_Integrator () {}

   /** Copy constructor, needed for backwards compatibility. */
   RKF45_Integrator (const RKF45_Integrator & src)
   :
      RKF45_IntegratorHelper (src),
      TrickFirstOrderOdeIntegrator (src, helper_integ_constructor)
   {}

   /** Non-default constructor, needed by getIntegrator. */
   RKF45_Integrator (int state_size, double delta_t)
   :
      RKF45_IntegratorHelper (),
      TrickFirstOrderOdeIntegrator (
         helper_integ_constructor, state_size, delta_t)
   {}

   /** Assignment operator. */
   RKF45_Integrator & operator= (RKF45_Integrator src)
   {
      helper_integ_constructor = src.helper_integ_constructor;
      TrickFirstOrderOdeIntegrator::swap (src);
      return *this;
   }

   /** Destructor. */
   virtual ~RKF45_Integrator() {}

   virtual Integrator_type get_Integrator_type() { return Runge_Kutta_Fehlberg_45; }
};

}

#endif

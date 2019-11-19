/**
 * @if Er7UtilsUseGroups
 * @addtogroup Er7Utils
 * @{
 * @addtogroup Trick
 * @{
 * @endif
 */

/**
 * @file
 * Defines the extensible class TrickFirstOrderOdeIntegrator, which extends the
 * TrickIntegrator class to integration techniques that are first order
 * by nature.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_TRICK_FIRST_ORDER_INTEGRATOR_HH
#define ER7_UTILS_TRICK_FIRST_ORDER_INTEGRATOR_HH

// System includes

// Integration includes
#include "er7_utils/integration/core/include/first_order_ode_integrator.hh"
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"

// Local includes
#include "trick_integrator.hh"


namespace er7_utils {

/**
 A TrickFirstOrderOdeIntegrator is-a TrickIntegrator that integrates state
 as a first order differential equation.
 */
class TrickFirstOrderOdeIntegrator : public TrickIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(TrickFirstOrderOdeIntegrator)

public:

   /**
    * Default constructor
    * This is needed for checkpoint/restart
    */
   TrickFirstOrderOdeIntegrator ()
   :
      TrickIntegrator ()
   {}

   /**
    * Copy constructor surrogate
    */
   TrickFirstOrderOdeIntegrator (
      const TrickFirstOrderOdeIntegrator & src,
      er7_utils::IntegratorConstructor & integ_constructor_in)
   :
      TrickIntegrator (src, integ_constructor_in)
   {}


   /**
    * Non-default, IntegratorConstructor-based constructor.
    */
   explicit TrickFirstOrderOdeIntegrator (
      IntegratorConstructor & integ_constructor_in)
   :
      TrickIntegrator (integ_constructor_in)
   {}


   /**
    * Non-default, fully-specified constructor.
    */
   TrickFirstOrderOdeIntegrator (
      IntegratorConstructor & integ_constructor_in,
      int state_size,
      double delta_t)
   :
      TrickIntegrator (integ_constructor_in, state_size, delta_t)
   {
      allocate_first_order_integrators (state_size);
   }


   /**
    * TrickFirstOrderOdeIntegrator destructor.
    */
   virtual ~TrickFirstOrderOdeIntegrator (
      void)
   {
      deallocate_integrators ();
   }


#ifndef SWIG

   // Integration group methods

   /**
    * Reset the integrator.
    */
   virtual void reset_body_integrators ()
   {
      first_order_integrator->reset_integrator ();
      if (second_order_integrator != NULL) {
         second_order_integrator->reset_integrator ();
      }
   }


   // Trick::Integrator methods

   /**
    * Initialize the integrator.
    * @param[in] state_size  Size of the state vector
    * @param[in] delta_t     Nominal simulation engine time step
    */
   virtual void initialize (
      int state_size,
      double delta_t)
   {
      TrickIntegrator::initialize (state_size, delta_t);
      allocate_first_order_integrators (state_size);
   }

   /**
    * Take the next integration step.
    */
   virtual int integrate () {

      if (intermediate_step == 0) {
         time_0 = time;
      }

      integ_mode = UseFirstOrderIntegrator;

      return integ_controls->integrate (
                time_0, dt,
                *this, *this, *this);
   }

#endif


protected:
   using BaseIntegrationGroup::initialize_group;
   using TrickIntegrator::integrate;


private:

   /**
    * Not implemented.
    */
   TrickFirstOrderOdeIntegrator (const TrickFirstOrderOdeIntegrator &);

   /**
    * Not implemented.
    */
   TrickFirstOrderOdeIntegrator & operator= (
      const TrickFirstOrderOdeIntegrator &);

};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

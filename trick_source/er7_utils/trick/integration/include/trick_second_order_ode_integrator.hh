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
 * Defines the extensible class TrickSecondOrderOdeIntegrator, which extends the
 * TrickIntegrator class to integration techniques that are first order
 * by nature.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_TRICK_SECOND_ORDER_INTEGRATOR_HH
#define ER7_UTILS_TRICK_SECOND_ORDER_INTEGRATOR_HH

// System includes

// Integrator includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"

// Local includes
#include "trick_integrator.hh"


namespace er7_utils {


/**
 A TrickSecondOrderOdeIntegrator is-a TrickIntegrator that integrates state
 as a second order differential equation.
 */
class TrickSecondOrderOdeIntegrator : public TrickIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(TrickSecondOrderOdeIntegrator)

public:

   /**
    * Default constructor
    * This is needed for checkpoint/restart
    */
   TrickSecondOrderOdeIntegrator()
   :
      TrickIntegrator ()
   {
      is_2nd_order_ODE_technique = true;
      use_deriv2 = false;
   }

   /**
    * Copy constructor surrogate
    */
   TrickSecondOrderOdeIntegrator (
      const TrickSecondOrderOdeIntegrator & src,
      IntegratorConstructor & integ_constructor_in)
   :
      TrickIntegrator (src, integ_constructor_in)
   {}

   /**
    * Non-default, IntegratorConstructor-based constructor.
    */
   explicit TrickSecondOrderOdeIntegrator (
      er7_utils::IntegratorConstructor & integ_constructor_in,
      bool use_deriv2_in = false)
   :
      TrickIntegrator (integ_constructor_in, use_deriv2_in)
   {
      is_2nd_order_ODE_technique = true;
   }

   /**
    * Non-default, fully-specified constructor.
    */
   TrickSecondOrderOdeIntegrator (
      er7_utils::IntegratorConstructor & integ_constructor_in,
      int state_size,
      double delta_t,
      bool use_deriv2_in = false)
   :
      TrickIntegrator (integ_constructor_in, state_size, delta_t, use_deriv2_in)
   {
      is_2nd_order_ODE_technique = true;

      allocate_second_order_integrators (state_size);
   }

   /**
    * Destructor.
    */
   ~TrickSecondOrderOdeIntegrator ()
   {
      deallocate_integrators ();
   }


#ifndef SWIG

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
      allocate_second_order_integrators (state_size);
   }

   /**
    * Take the next integration step.
    * @return non-zero step number or zero upon completion.
    */
   virtual int integrate ()
   {

      if (intermediate_step == 0) {
         time_0 = time;
      }

      integ_mode = UseSecondOrderIntegrator;

      return integ_controls->integrate (
                time_0, dt,
                *this, *this, *this);
   }


   /**
    * Get the use_deriv2 flag
    * @return              Value of use_deriv2
    */
   bool get_use_deriv2 () { return use_deriv2; }


protected:
   using BaseIntegrationGroup::initialize_group;
   using TrickIntegrator::integrate;

   /**
    * Set the use_deriv2 flag
    * @param value         Value to be assigned to use_deriv2
    */
   void set_use_deriv2 (bool value) { use_deriv2 = value; }

#endif

private:


   /**
    * Not implemented.
    */
   TrickSecondOrderOdeIntegrator (const TrickSecondOrderOdeIntegrator &);


   /**
    * Not implemented.
    */
   TrickSecondOrderOdeIntegrator & operator= (
      const TrickSecondOrderOdeIntegrator &);

};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

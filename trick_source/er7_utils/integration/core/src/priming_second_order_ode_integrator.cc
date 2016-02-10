/**
 * @if Er7UtilsUseGroups
 * @addtogroup Er7Utils
 * @{
 * @addtogroup Integration
 * @{
 * @endif
 */

/**
 * @file
 * Defines member functions for the class PrimingSecondOrderODEIntegrator.
 */

/*
Purpose: ()
*/

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integrator_constructor.hh"
#include "er7_utils/integration/core/include/integration_controls.hh"

// Model includes
#include "../include/priming_second_order_ode_integrator.hh"
#include "../include/priming_integration_controls.hh"


namespace er7_utils {

// Default constructor.
PrimingSecondOrderODEIntegrator::PrimingSecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   controls (NULL),
   primer (NULL),
   priming_state_size (0),
   prime_counter (0),
   primed (false)
{ }

// Copy constructor.
PrimingSecondOrderODEIntegrator::PrimingSecondOrderODEIntegrator (
   const PrimingSecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (src),
   controls (NULL),
   primer (src.primer->create_copy()),
   priming_state_size (src.priming_state_size),
   prime_counter (src.prime_counter),
   primed (src.primed)
{ }


// Non-default constructor for a PrimingSecondOrderODEIntegrator.
PrimingSecondOrderODEIntegrator::PrimingSecondOrderODEIntegrator (
   unsigned int priming_size,
   const IntegratorConstructor & primer_constructor,
   unsigned int size,
   IntegrationControls & controls_in)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls_in),
   controls (&controls_in),
   primer (NULL),
   priming_state_size (priming_size),
   prime_counter (priming_size),
   primed (false)
{
   // Create the state integrator used to prime the integrator.
   primer = primer_constructor.create_second_order_ode_integrator (
               size, controls_in);
}


// Non-default constructor for a PrimingSecondOrderODEIntegrator
// for generalized position, generalized velocity.
PrimingSecondOrderODEIntegrator::PrimingSecondOrderODEIntegrator (
   unsigned int priming_size,
   const IntegratorConstructor & primer_constructor,
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls_in)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             deriv_funs, controls_in),
   controls (&controls_in),
   primer (NULL),
   priming_state_size (priming_size),
   prime_counter (priming_size),
   primed (false)
{
   // Create the integrator used to prime the state.
   primer =
      primer_constructor.create_generalized_deriv_second_order_ode_integrator (
         position_size, velocity_size, deriv_funs, controls_in);
}


// Non-default constructor for a PrimingSecondOrderODEIntegrator
// for generalized position, generalized velocity.
PrimingSecondOrderODEIntegrator::PrimingSecondOrderODEIntegrator (
   unsigned int priming_size,
   const IntegratorConstructor & primer_constructor,
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls_in)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             step_funs, controls_in),
   controls (&controls_in),
   primer (NULL),
   priming_state_size (priming_size),
   prime_counter (priming_size),
   primed (false)
{
   // Create the integrator used to prime the state.
   primer =
      primer_constructor.create_generalized_step_second_order_ode_integrator (
         position_size, velocity_size, step_funs, controls_in);
}


// PrimingSecondOrderODEIntegrator destructor.
PrimingSecondOrderODEIntegrator::~PrimingSecondOrderODEIntegrator (
   void)
{
   // Delete the primer.
   alloc::delete_object (primer);
}


// Non-throwing swap.
void
PrimingSecondOrderODEIntegrator::swap (
   PrimingSecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);
   std::swap (controls, other.controls);
   std::swap (primer, other.primer);
   std::swap (priming_state_size, other.priming_state_size);
   std::swap (prime_counter, other.prime_counter);
   std::swap (primed, other.primed);
}


// Set the integration controls.
void
PrimingSecondOrderODEIntegrator::set_controls (
   IntegrationControls & controls_in)
{
   controls = &controls_in;

   PrimingIntegrationControls * priming_controls =
      dynamic_cast<PrimingIntegrationControls*> (controls);
   if ((priming_controls == NULL) || (primer == NULL)) {
      // FIXME: Error message
      return;
   }

   primer->set_controls (priming_controls->get_priming_controls());
}


// Integrate state
IntegratorResult
PrimingSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   const double * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{

   /**
    * ### Overview
    *
    * Multistep integration techniques use a recent history of derivatives
    * at the start of some number of integration cycles to propagate state.
    * These techniques need to be primed (bootstrapped) by some other
    * integration technique to establish that derivative history.
    *
    * Suppose some multistep technique needs two start of cycle derivatives.
    * Because derivatives are provided as input to the integrate function,
    * the first such derivative is "free". The primer must be driven through
    * a complete integration cycle so as to obtain the needed second set of
    * derivatives. In general, a technique that uses a history of N derivatives
    * needs to have the primer driven through N-1 complete cycles.
    *
    * This function controls the integration process, driving the primer through
    * priming_state_size-1 integration cycles so as to establish the requisite
    * priming_state_size start of cycle derivatives.
    * The technique itself is used once the integrator has been primed.
    *
    * ### Algorithm
    */


   /**
    * If priming isn't complete,
    */
   if (! primed) {
      /**
       *  - At the start of an integration cycle,
       *    + Decrement the countdown.
       *    + Save the derivatives if the countdown hasn't reached zero.
       *    + Priming is finished when the countdown reaches zero.
       */
      if (controls->in_initial_cycle()) {
         --prime_counter;
         if (prime_counter > 0) {
            technique_save_derivatives (
               prime_counter, accel, velocity, position);
         }
         else {
            primed = true;
         }
      }

      /**
       *  - Integrate state via the primer and return if priming isn't complete.
       */
      if (! primed) {
         return primer->integrate (
                   dyn_dt, target_stage, accel, velocity, position);
      }
   }

   /**
    * Use the technique-specific integrator the once priming is complete.
    */
   return technique_integrate (dyn_dt, target_stage, accel, velocity, position);
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

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
 * Defines member functions for the class ABM4FirstOrderODEIntegrator.
 */

/*
Purpose: ()
*/

// System includes
#include <algorithm>
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/abm_utils.hh"
#include "er7_utils/integration/core/include/integ_utils.hh"
#include "er7_utils/integration/core/include/integrator_constructor.hh"
#include "er7_utils/integration/core/include/integration_controls.hh"

// Model includes
#include "../include/abm4_first_order_ode_integrator.hh"


namespace er7_utils {

// Default constructor.
ABM4FirstOrderODEIntegrator::ABM4FirstOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   PrimingFirstOrderODEIntegrator (),
   init_state (NULL)
{
   alloc::initialize_2D_array<double, 4> (deriv_hist);
}

// Copy constructor.
ABM4FirstOrderODEIntegrator::ABM4FirstOrderODEIntegrator (
   const ABM4FirstOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   PrimingFirstOrderODEIntegrator (src),
   init_state (NULL)
{
   // Replicate the source's contents if they exist.
   if (src.init_state != NULL) {
      init_state = alloc::replicate_array<double> (state_size, src.init_state);
      alloc::replicate_2D_array<double, 4> (state_size, src.deriv_hist,
                                            deriv_hist);
   }
   else {
      alloc::initialize_2D_array<double, 4> (deriv_hist);
   }
}


// Non-default constructor for a ABM4FirstOrderODEIntegrator.
ABM4FirstOrderODEIntegrator::ABM4FirstOrderODEIntegrator (
   const IntegratorConstructor & primer_constructor,
   unsigned int size,
   IntegrationControls & controls_in)
:
   Er7UtilsDeletable (),
   PrimingFirstOrderODEIntegrator (4, primer_constructor, size, controls_in),
   init_state(NULL)
{
   // Allocate storage for the ABM method.
   init_state = alloc::allocate_array<double> (state_size);
   alloc::allocate_2D_array<double, 4> (state_size, deriv_hist);
}


// ABM4FirstOrderODEIntegrator destructor.
ABM4FirstOrderODEIntegrator::~ABM4FirstOrderODEIntegrator (
   void)
{
   // Free ABM storage memory.
   alloc::deallocate_array<double> (init_state);
   alloc::deallocate_2D_array<double, 4> (deriv_hist);
}


// Clone a ABM4FirstOrderODEIntegrator.
ABM4FirstOrderODEIntegrator *
ABM4FirstOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Non-throwing swap.
void
ABM4FirstOrderODEIntegrator::swap (
   ABM4FirstOrderODEIntegrator & other)
{
   PrimingFirstOrderODEIntegrator::swap (other);
   std::swap (init_state, other.init_state);
   std::swap (deriv_hist[0], other.deriv_hist[0]);
   std::swap (deriv_hist[1], other.deriv_hist[1]);
   std::swap (deriv_hist[2], other.deriv_hist[2]);
   std::swap (deriv_hist[3], other.deriv_hist[3]);
}


//Save derivatives during priming.
void
ABM4FirstOrderODEIntegrator::technique_save_derivatives (
   int,
   const double * ER7_UTILS_RESTRICT velocity,
   const double *)
{
   abm::rotate_history<4> (deriv_hist);
   integ_utils::copy_array (velocity, state_size, deriv_hist[0]);
}


// Integrate state using ABM4 proper.
IntegratorResult
ABM4FirstOrderODEIntegrator::technique_integrate (
   double dt,
   unsigned int target_stage,
   const double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   static const double wscale = 1.0/24.0;
   static const double predictor_weights[4] = {55.0, -59.0, 37.0, -9.0};
   static const double corrector_weights[4] = {9.0, 19.0, -5.0, 1.0};

   /**
    * ### Overview
    *
    * ABM4 proper requires four sets of start of cycle derivatives.
    * This means that ABM4 needs to be "primed" by some other integration
    * technique. The primer is run through three complete cycles to generate
    * the requisite four sets.
    *
    * ABM4 itself is used once the integrator has been primed. ABM4 integrates
    * state by using fourth order Adams Bashforth as a predictor and third order
    * Adams Moulton as a corrector.
    *
    * ### Algorithm
    */

   /**
    * Integrate state via ABM4 once priming is complete.
    * Advance state based on the target stage number.
    */
   switch (target_stage) {

   /**
    * - ABM4 target stage 1 advances state to the end of the integration
    *   interval using fourth order Adams-Bashforth as a predictor.
    *   + Rotate the derivatives history. This rotation moves the oldest
    *     set of derivatives to the zeroth position.
    *   + Integrate state via fourth order Adams Bashforth,
    *     saving derivatives in the zeroth element of the derivatives
    *     history.
    */
   case 1:
      abm::rotate_history<4> (deriv_hist);

      abm::predictor_step<4> (velocity, predictor_weights,
                              wscale, dt, state_size,
                              deriv_hist, init_state, position);
      break;


   /**
    * - ABM4 target stage 2 advances the state to the end of the integration
    *   interval using the third order Adams-Moulton method as a corrector.
    */
   case 2:

      abm::corrector_step<4> (init_state, velocity, deriv_hist,
                              corrector_weights, wscale, dt, state_size,
                              position);
      break;
   }

   return 1.0;
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

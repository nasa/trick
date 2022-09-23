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
 * Defines member functions for the class SecondOrderODEIntegrator.
 */

/*
Purpose: ()
*/

// System includes
#include <algorithm>
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/message_handler.hh"

// Local includes
#include "../include/integration_messages.hh"
#include "../include/second_order_ode_integrator.hh"


namespace er7_utils {

// SecondOrderODEIntegrator default constructor.
SecondOrderODEIntegrator::SecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   StateIntegratorInterface (),
   compute_posdot (NULL),
   compute_posdotdot (NULL),
   compute_expmap_position_step (NULL),
   compute_dexpinv_velocity_transform (NULL),
   problem_type (Integration::SimpleSecondOrderODE)
{
   // Initialize the sizes.
   state_size[0] = state_size[1] = 0;
}

// SecondOrderODEIntegrator copy constructor.
SecondOrderODEIntegrator::SecondOrderODEIntegrator (
   const SecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   StateIntegratorInterface (src),
   compute_posdot (src.compute_posdot),
   compute_posdotdot (src.compute_posdotdot),
   compute_expmap_position_step (src.compute_expmap_position_step),
   compute_dexpinv_velocity_transform (src.compute_dexpinv_velocity_transform),
   problem_type (src.problem_type)
{
   // Initialize the sizes.
   state_size[0] = src.state_size[0];
   state_size[1] = src.state_size[1];
}


// SecondOrderODEIntegrator non-default constructor.
SecondOrderODEIntegrator::SecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls &)
:
   Er7UtilsDeletable (),
   StateIntegratorInterface(),
   compute_posdot (NULL),
   compute_posdotdot (NULL),
   compute_expmap_position_step (NULL),
   compute_dexpinv_velocity_transform (NULL),
   problem_type (Integration::SimpleSecondOrderODE)
{
   // Save the provided sizes.
   state_size[0] = state_size[1] = size;
}


// SecondOrderODEIntegrator non-default constructor.
SecondOrderODEIntegrator::SecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls &)
:
   Er7UtilsDeletable (),
   StateIntegratorInterface (),
   compute_posdot (deriv_funs.first_deriv_fun),
   compute_posdotdot (deriv_funs.second_deriv_fun),
   compute_expmap_position_step (NULL),
   compute_dexpinv_velocity_transform (NULL),
   problem_type (Integration::GeneralizedDerivSecondOrderODE)
{
   // The derivative functions must not be null.
   if ((compute_posdot == NULL) || (compute_posdotdot == NULL)) {
      MessageHandler::fail (
         __FILE__, __LINE__,
         IntegrationMessages::invalid_request,
         "The function pointers provided to\n"
         "the SecondOrderODEIntegrator constructor must be non-null.");
   }

   // Save the provided sizes.
   state_size[0] = position_size;
   state_size[1] = velocity_size;
}


// SecondOrderODEIntegrator non-default constructor.
SecondOrderODEIntegrator::SecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls &)
:
   Er7UtilsDeletable (),
   StateIntegratorInterface (),
   compute_posdot (NULL),
   compute_posdotdot (NULL),
   compute_expmap_position_step (step_funs.expmap_step_fun),
   compute_dexpinv_velocity_transform (step_funs.dexpinv_xform_fun),
   problem_type (Integration::GeneralizedStepSecondOrderODE)
{
   // The step functions must not be null.
   if ((compute_expmap_position_step == NULL) ||
       (compute_dexpinv_velocity_transform == NULL)) {
      MessageHandler::fail (
         __FILE__, __LINE__,
         IntegrationMessages::invalid_request,
         "The function pointers provided to\n"
         "the SecondOrderODEIntegrator constructor must be non-null.");
   }

   // Save the provided sizes.
   state_size[0] = position_size;
   state_size[1] = velocity_size;
}


// Reset the position derivative functions.
void
SecondOrderODEIntegrator::set_position_derivative_functions (
   const GeneralizedPositionDerivativeFunctions & deriv_funs)
{
   if ((deriv_funs.first_deriv_fun == NULL) ||
       (deriv_funs.second_deriv_fun == NULL)) {
      MessageHandler::fail (
         __FILE__, __LINE__,
         IntegrationMessages::invalid_request,
         "The derivative function pointers provided to\n"
         "set_position_derivative_functions must be non-null.");
   }

   compute_posdot = deriv_funs.first_deriv_fun;
   compute_posdotdot = deriv_funs.second_deriv_fun;
}


// Reset the position step functions.
void
SecondOrderODEIntegrator::set_position_step_functions (
   const GeneralizedPositionStepFunctions & step_funs)
{
   if ((step_funs.expmap_step_fun == NULL) ||
       (step_funs.dexpinv_xform_fun == NULL)) {
      MessageHandler::fail (
         __FILE__, __LINE__,
         IntegrationMessages::invalid_request,
         "The step function pointers provided to\n"
         "set_position_step_functions must be non-null.");
   }

   compute_expmap_position_step = step_funs.expmap_step_fun;
   compute_dexpinv_velocity_transform = step_funs.dexpinv_xform_fun;
}


// Swap
void
SecondOrderODEIntegrator::swap (
   SecondOrderODEIntegrator & other)
{
   std::swap (compute_posdot, other.compute_posdot);
   std::swap (compute_posdotdot, other.compute_posdotdot);
   std::swap (compute_expmap_position_step,
              other.compute_expmap_position_step);
   std::swap (compute_dexpinv_velocity_transform,
              other.compute_dexpinv_velocity_transform);
   std::swap (state_size[0], other.state_size[0]);
   std::swap (state_size[1], other.state_size[1]);
}



}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

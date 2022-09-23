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
 * Defines member functions for the class RK2MidpointSecondOrderODEIntegrator.
 */

/*
Purpose: ()
*/

// System includes

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integ_utils.hh"
#include "er7_utils/integration/core/include/rk_utils.hh"

// Model includes
#include "../include/rk2_midpoint_second_order_ode_integrator.hh"


namespace er7_utils {

// RK2MidpointSecondOrderODEIntegrator default constructor
RK2MidpointSecondOrderODEIntegrator::RK2MidpointSecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   init_pos (NULL),
   init_vel (NULL),
   posdot (NULL),
   dtheta (NULL)
{ }


// Copy constructor.
RK2MidpointSecondOrderODEIntegrator::RK2MidpointSecondOrderODEIntegrator (
   const RK2MidpointSecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (src),
   init_pos (NULL),
   init_vel (NULL),
   posdot (NULL),
   dtheta (NULL)
{
   if (problem_type == Integration::GeneralizedStepSecondOrderODE) {
      init_pos = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      posdot   = alloc::replicate_array (state_size[1], src.posdot);
      dtheta   = alloc::replicate_array (state_size[1], src.dtheta);
   }

   else if (problem_type == Integration::GeneralizedDerivSecondOrderODE) {
      init_pos = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      posdot   = alloc::replicate_array (state_size[0], src.posdot);
   }

   if (init_pos != NULL) {
      init_pos = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
   }
}


// Non-default constructor for an RK2MidpointSecondOrderODEIntegrator
// in which the time derivative of position is the generalized velocity.
RK2MidpointSecondOrderODEIntegrator::RK2MidpointSecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   init_pos (NULL),
   init_vel (NULL),
   posdot (NULL),
   dtheta (NULL)
{
   // Allocate memory used by Runge Kutta 2 algorithm.
   init_pos = alloc::allocate_array<double> (size);
   init_vel = alloc::allocate_array<double> (size);
}


// Non-default constructor for an RK2MidpointSecondOrderODEIntegrator
// for generalized position, generalized velocity.
RK2MidpointSecondOrderODEIntegrator::RK2MidpointSecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             deriv_funs, controls),
   init_pos (NULL),
   init_vel (NULL),
   posdot (NULL),
   dtheta (NULL)
{
   // Allocate memory used by Runge Kutta 2 algorithm.
   init_pos = alloc::allocate_array<double> (position_size);
   init_vel = alloc::allocate_array<double> (velocity_size);
   posdot = alloc::allocate_array<double> (position_size);
}


// Non-default constructor for an RK2MidpointSecondOrderODEIntegrator
// for generalized position, generalized velocity.
RK2MidpointSecondOrderODEIntegrator::RK2MidpointSecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             step_funs, controls),
   init_pos (NULL),
   init_vel (NULL),
   posdot (NULL),
   dtheta (NULL)
{
   // Allocate memory used by Runge Kutta 2 algorithm.
   init_pos = alloc::allocate_array<double> (position_size);
   init_vel = alloc::allocate_array<double> (velocity_size);
   posdot = alloc::allocate_array<double> (velocity_size);
   dtheta = alloc::allocate_array<double> (velocity_size);
}


// RK2MidpointSecondOrderODEIntegrator destructor.
RK2MidpointSecondOrderODEIntegrator::~RK2MidpointSecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array<double> (init_pos);
   alloc::deallocate_array<double> (init_vel);
   alloc::deallocate_array<double> (posdot);
   alloc::deallocate_array<double> (dtheta);
}


// Non-throwing swap.
void
RK2MidpointSecondOrderODEIntegrator::swap (
   RK2MidpointSecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (init_pos, other.init_pos);
   std::swap (init_vel, other.init_vel);
   std::swap (posdot, other.posdot);
   std::swap (dtheta, other.dtheta);
}


// Clone a RK2MidpointSimpleSecondOrderODEIntegrator.
RK2MidpointSimpleSecondOrderODEIntegrator *
RK2MidpointSimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RK2MidpointGeneralizedDerivSecondOrderODEIntegrator.
RK2MidpointGeneralizedDerivSecondOrderODEIntegrator *
RK2MidpointGeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RK2MidpointGeneralizedStepSecondOrderODEIntegrator.
RK2MidpointGeneralizedStepSecondOrderODEIntegrator *
RK2MidpointGeneralizedStepSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
RK2MidpointSimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   // Advance the state.
   switch (target_stage) {
   case 1:
      integ_utils::inplace_two_state_euler_step_save_pos_vel (
         accel, 0.5*dyn_dt, state_size[0],
         init_pos, init_vel, position, velocity);
      step_factor = 0.5;
      break;

   case 2:
      integ_utils::two_state_euler_step (
         init_pos, init_vel, accel, dyn_dt, state_size[0],
         position, velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


// Propagate state with the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
RK2MidpointGeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   // Compute time derivative of canonical position,
   // saving the result in the position derivative history.
   compute_posdot (position, velocity, posdot);

   // Advance the state.
   switch (target_stage) {
   case 1:
      integ_utils::inplace_two_state_euler_step_save_pos_vel (
         posdot, accel, 0.5*dyn_dt, state_size,
         init_pos, init_vel, position, velocity);
      step_factor = 0.5;
      break;

   case 2:
      integ_utils::two_state_euler_step (
         init_pos, init_vel, posdot, accel, dyn_dt, state_size,
         position, velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


// Propagate state with the generalized position being
// advanced via the external position step function.
IntegratorResult
RK2MidpointGeneralizedStepSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   // Advance the state.
   switch (target_stage) {
   case 1: {
      double hdt = 0.5*dyn_dt;

      // Integrate generalized position.
      integ_utils::copy_array (position, state_size[0], init_pos);
      integ_utils::scale_array (velocity, hdt, state_size[1], dtheta);
      compute_expmap_position_step (init_pos, dtheta, position);

      // Integrate generalized velocity.
      integ_utils::inplace_euler_step_save_state (
         accel, hdt, state_size[1], init_vel, velocity);
      step_factor = 0.5;
      break;
   }

   case 2:

      // Transform the velocity via dexpinv using the previous generalized step.
      compute_dexpinv_velocity_transform (velocity, dtheta, posdot);

      // Integrate generalized position.
      integ_utils::scale_array (posdot, dyn_dt, state_size[1], dtheta);
      compute_expmap_position_step (init_pos, dtheta, position);

      // Integrate generalized velocity.
      integ_utils::euler_step (
         init_vel, accel, dyn_dt, state_size[1], velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

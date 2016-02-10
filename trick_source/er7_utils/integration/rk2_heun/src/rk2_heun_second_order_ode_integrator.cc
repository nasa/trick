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
 * Defines member functions for the class RK2HeunSecondOrderODEIntegrator.
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
#include "../include/rk2_heun_second_order_ode_integrator.hh"


namespace er7_utils {

// RK2HeunSecondOrderODEIntegrator default constructor
RK2HeunSecondOrderODEIntegrator::RK2HeunSecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   init_pos (NULL),
   init_vel (NULL),
   init_acc (NULL),
   dtheta (NULL),
   init_posdot (NULL),
   final_posdot (NULL)
{
}

// RK2HeunSecondOrderODEIntegrator copy constructor
RK2HeunSecondOrderODEIntegrator::RK2HeunSecondOrderODEIntegrator (
   const RK2HeunSecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   init_pos (NULL),
   init_vel (NULL),
   init_acc (NULL),
   dtheta (NULL),
   init_posdot (NULL),
   final_posdot (NULL)
{
   // Replicate the source's contents if they exist.
   if (problem_type == Integration::GeneralizedStepSecondOrderODE) {
      init_pos     = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel     = alloc::replicate_array (state_size[1], src.init_vel);
      init_acc     = alloc::replicate_array (state_size[1], src.init_acc);
      dtheta       = alloc::replicate_array (state_size[1], src.dtheta);
      final_posdot = alloc::replicate_array (state_size[1], src.final_posdot);
   }

   else if (problem_type == Integration::GeneralizedDerivSecondOrderODE) {
      init_pos     = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel     = alloc::replicate_array (state_size[1], src.init_vel);
      init_acc     = alloc::replicate_array (state_size[1], src.init_acc);
      init_posdot  = alloc::replicate_array (state_size[0], src.init_posdot);
      final_posdot = alloc::replicate_array (state_size[0], src.final_posdot);
   }

   else if (src.init_pos != NULL) {
      init_pos = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      init_acc = alloc::replicate_array (state_size[1], src.init_acc);
   }
}


// Non-default constructor for an RK2HeunSecondOrderODEIntegrator
// in which the time derivative of position is the generalized velocity.
RK2HeunSecondOrderODEIntegrator::RK2HeunSecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   init_pos (NULL),
   init_vel (NULL),
   init_acc (NULL),
   dtheta (NULL),
   init_posdot (NULL),
   final_posdot (NULL)
{
   // Allocate memory used by Runge Kutta 2 algorithm.
   init_pos = alloc::allocate_array<double> (size);
   init_vel = alloc::allocate_array<double> (size);
   init_acc = alloc::allocate_array<double> (size);
}


// Non-default constructor for an RK2HeunSecondOrderODEIntegrator
// for generalized position, generalized velocity.
RK2HeunSecondOrderODEIntegrator::RK2HeunSecondOrderODEIntegrator (
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
   init_acc (NULL),
   dtheta (NULL),
   init_posdot (NULL),
   final_posdot (NULL)
{
   // Allocate memory used by Runge Kutta 2 algorithm.
   init_pos = alloc::allocate_array<double> (position_size);
   init_vel = alloc::allocate_array<double> (velocity_size);
   init_acc = alloc::allocate_array<double> (velocity_size);
   init_posdot = alloc::allocate_array<double> (position_size);
   final_posdot = alloc::allocate_array<double> (position_size);
}


// Non-default constructor for an RK2HeunSecondOrderODEIntegrator
// for generalized position, generalized velocity.
RK2HeunSecondOrderODEIntegrator::RK2HeunSecondOrderODEIntegrator (
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
   init_acc (NULL),
   dtheta (NULL),
   init_posdot (NULL),
   final_posdot (NULL)
{
   // Allocate memory used by Runge Kutta 2 algorithm.
   init_pos = alloc::allocate_array<double> (position_size);
   init_vel = alloc::allocate_array<double> (velocity_size);
   init_acc = alloc::allocate_array<double> (velocity_size);
   dtheta = alloc::allocate_array<double> (velocity_size);
   final_posdot = alloc::allocate_array<double> (velocity_size);
}


// RK2HeunSecondOrderODEIntegrator destructor.
RK2HeunSecondOrderODEIntegrator::~RK2HeunSecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array<double> (init_pos);
   alloc::deallocate_array<double> (init_vel);
   alloc::deallocate_array<double> (init_acc);
   alloc::deallocate_array<double> (dtheta);
   alloc::deallocate_array<double> (init_posdot);
   alloc::deallocate_array<double> (final_posdot);
}


// Non-throwing swap.
void
RK2HeunSecondOrderODEIntegrator::swap (
   RK2HeunSecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (init_pos, other.init_pos);
   std::swap (init_vel, other.init_vel);
   std::swap (init_acc, other.init_acc);
   std::swap (dtheta, other.dtheta);
   std::swap (init_posdot, other.init_posdot);
   std::swap (final_posdot, other.final_posdot);
}


// Clone a RK2HeunSimpleSecondOrderODEIntegrator.
RK2HeunSimpleSecondOrderODEIntegrator *
RK2HeunSimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RK2HeunGeneralizedDerivSecondOrderODEIntegrator.
RK2HeunGeneralizedDerivSecondOrderODEIntegrator *
RK2HeunGeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RK2HeunGeneralizedStepSecondOrderODEIntegrator.
RK2HeunGeneralizedStepSecondOrderODEIntegrator *
RK2HeunGeneralizedStepSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
RK2HeunSimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   // Advance state per Heun's method.
   switch (target_stage) {
   case 1:
      integ_utils::inplace_two_state_euler_step_save_pos_vel_acc (
         accel, dyn_dt, state_size[0],
         init_pos, init_vel, init_acc, position, velocity);
      break;

   case 2: {
      int lim = state_size[0];
      double hdt = 0.5 * dyn_dt;
      for (int ii = 0; ii < lim; ++ii) {
         position[ii] = init_pos[ii] + (init_vel[ii] + velocity[ii]) * hdt;
         velocity[ii] = init_vel[ii] + (init_acc[ii] + accel[ii]) * hdt;
      }
      break;
   }

   default:
      break;
   }

   return 1.0;
}


// Propagate state with the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
RK2HeunGeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   // Advance state per Heun's method.
   switch (target_stage) {
   case 1:
      compute_posdot (position, velocity, init_posdot);
      integ_utils::inplace_two_state_euler_step_save_pos_vel_acc (
         init_posdot, accel, dyn_dt, state_size,
         init_pos, init_vel, init_acc, position, velocity);
      break;

   case 2:
      compute_posdot (position, velocity, final_posdot);
      rk::rk2_heun_final_step (
         init_pos, init_posdot, final_posdot, dyn_dt, state_size[0], position);
      rk::rk2_heun_final_step (
         init_vel, init_acc, accel, dyn_dt, state_size[1], velocity);
      break;

   default:
      break;
   }

   return 1.0;
}


// Propagate state with the generalized position being
// advanced via the external position step function.
IntegratorResult
RK2HeunGeneralizedStepSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   // Advance state per Heun's method.
   switch (target_stage) {
   case 1:
      integ_utils::copy_array (position, state_size[0], init_pos);
      integ_utils::scale_array (velocity, dyn_dt, state_size[1], dtheta);
      compute_expmap_position_step (init_pos, dtheta, position);

      integ_utils::inplace_euler_step_save_both (
         accel, dyn_dt, state_size[1], init_vel, init_acc, velocity);
      break;

   case 2: {
      double hdt = 0.5 * dyn_dt;

      compute_dexpinv_velocity_transform (velocity, dtheta, final_posdot);
      for (int ii = 0, lim = state_size[1]; ii < lim; ++ii) {
         dtheta[ii] = (init_vel[ii] + final_posdot[ii]) * hdt;
      }
      compute_expmap_position_step (init_pos, dtheta, position);

      rk::rk2_heun_final_step (
         init_vel, init_acc, accel, dyn_dt, state_size[1], velocity);
      break;
   }

   default:
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

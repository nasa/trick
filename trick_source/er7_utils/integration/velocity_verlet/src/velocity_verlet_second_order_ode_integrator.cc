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
 * Defines member functions for the class
 * VelocityVerletSecondOrderODEIntegrator.
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
#include "er7_utils/integration/core/include/integ_utils.hh"

// Model includes
#include "../include/velocity_verlet_second_order_ode_integrator.hh"


namespace er7_utils {

// VelocityVerletSecondOrderODEIntegrator default constructor.
VelocityVerletSecondOrderODEIntegrator::
VelocityVerletSecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   init_vel (NULL),
   init_acc (NULL),
   dtheta   (NULL),
   posdot   (NULL)
{
}


// VelocityVerletSecondOrderODEIntegrator copy constructor
VelocityVerletSecondOrderODEIntegrator::VelocityVerletSecondOrderODEIntegrator (
   const VelocityVerletSecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   init_vel (NULL),
   init_acc (NULL),
   dtheta   (NULL),
   posdot   (NULL)
{
   // Replicate the source's contents if they exist.
   if (problem_type == Integration::GeneralizedStepSecondOrderODE) {
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      init_acc = alloc::replicate_array (state_size[1], src.init_acc);
      dtheta   = alloc::replicate_array (state_size[1], src.dtheta);
   }

   else if (problem_type == Integration::GeneralizedDerivSecondOrderODE) {
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      init_acc = alloc::replicate_array (state_size[1], src.init_acc);
      dtheta   = alloc::replicate_array (state_size[1], src.dtheta);
      posdot   = alloc::replicate_array (state_size[0], src.posdot);
   }

   else if (src.init_vel != NULL) {
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      init_acc = alloc::replicate_array (state_size[1], src.init_acc);
   }
}

// Non-default constructor for an VelocityVerletSecondOrderODEIntegrator
// in which the time derivative of position is the generalized velocity.
VelocityVerletSecondOrderODEIntegrator::
VelocityVerletSecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   init_vel (NULL),
   init_acc (NULL),
   dtheta   (NULL),
   posdot   (NULL)
{
   // Allocate memory used by simple velocity verlet.
   init_vel = alloc::allocate_array (size);
   init_acc = alloc::allocate_array (size);
}


// Non-default constructor for an VelocityVerletSecondOrderODEIntegrator
// for generalized position, generalized velocity.
VelocityVerletSecondOrderODEIntegrator::
VelocityVerletSecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             deriv_funs, controls),
   init_vel (NULL),
   init_acc (NULL),
   dtheta   (NULL),
   posdot   (NULL)
{
   // Allocate memory used by generalized deriv velocity verlet.
   init_vel = alloc::allocate_array (velocity_size);
   init_acc = alloc::allocate_array (velocity_size);
   dtheta   = alloc::allocate_array (velocity_size);
   posdot   = alloc::allocate_array (position_size);
}


// Non-default constructor for an VelocityVerletSecondOrderODEIntegrator
// for generalized position, generalized velocity.
VelocityVerletSecondOrderODEIntegrator::
VelocityVerletSecondOrderODEIntegrator (
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (position_size, velocity_size,
                             step_funs, controls),
   init_vel (NULL),
   init_acc (NULL),
   dtheta   (NULL),
   posdot   (NULL)
{
   // Allocate memory used by generalized step velocity verlet.
   init_vel = alloc::allocate_array (velocity_size);
   init_acc = alloc::allocate_array (velocity_size);
   dtheta   = alloc::allocate_array (velocity_size);
}


// VelocityVerletSecondOrderODEIntegrator destructor.
VelocityVerletSecondOrderODEIntegrator::
~VelocityVerletSecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array (init_vel);
   alloc::deallocate_array (init_acc);
   alloc::deallocate_array (dtheta);
   alloc::deallocate_array (posdot);
}


// Non-throwing swap.
void
VelocityVerletSecondOrderODEIntegrator::swap (
   VelocityVerletSecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (init_vel, other.init_vel);
   std::swap (init_acc, other.init_acc);
   std::swap (dtheta, other.dtheta);
   std::swap (posdot, other.posdot);
}


// Clone a VelocityVerletSimpleSecondOrderODEIntegrator.
VelocityVerletSimpleSecondOrderODEIntegrator *
VelocityVerletSimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a VelocityVerletGeneralizedDerivSecondOrderODEIntegrator.
VelocityVerletGeneralizedDerivSecondOrderODEIntegrator *
VelocityVerletGeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a VelocityVerletGeneralizedStepSecondOrderODEIntegrator.
VelocityVerletGeneralizedStepSecondOrderODEIntegrator *
VelocityVerletGeneralizedStepSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}



/**
 * Advance position and velocity for the 1st step of velocity verlet.
 * @param[in]     accel     Initial acceleration
 * @param[in]     deltat    Time step
 * @param[in]     size      State size
 * @param[in,out] position  Updated position vector
 * @param[in,out] velocity  Updated velocity vector
 * @param[out]    init_vel  Saved initial velocity vector
 * @param[out]    init_acc  Saved initial acceleration vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
simple_velocity_verlet_step_one (
   double const * ER7_UTILS_RESTRICT accel,
   double deltat,
   int size,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT init_vel,
   double * ER7_UTILS_RESTRICT init_acc)
{
   double hdt = 0.5*deltat;

   for (int ii = 0; ii < size; ++ii) {
      double hdv = accel[ii]*hdt;
      double vmid = velocity[ii] + hdv;
      init_acc[ii]  = accel[ii];
      init_vel[ii]  = velocity[ii];
      velocity[ii]  = vmid + hdv;
      position[ii] += vmid * deltat;
   }
}


/**
 * Advance position and velocity for the 1st step of velocity verlet.
 * @param[in]     accel     Initial acceleration
 * @param[in]     deltat    Time step
 * @param[in]     size      State size
 * @param[out]    vel_mid   Velocity at the midpoint
 * @param[in,out] velocity  Updated velocity vector
 * @param[out]    init_vel  Saved initial velocity vector
 * @param[out]    init_acc  Saved initial acceleration vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
generalized_deriv_velocity_verlet_step_one (
   double const * ER7_UTILS_RESTRICT accel,
   double deltat,
   int size,
   double * ER7_UTILS_RESTRICT vel_mid,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT init_vel,
   double * ER7_UTILS_RESTRICT init_acc)
{
   double hdt = 0.5*deltat;

   for (int ii = 0; ii < size; ++ii) {
      double hdv = accel[ii]*hdt;
      double vmid = velocity[ii] + hdv;
      init_acc[ii]  = accel[ii];
      init_vel[ii]  = velocity[ii];
      velocity[ii]  = vmid + hdv;
      vel_mid[ii]   = vmid;
   }
}


/**
 * Advance position and velocity for the 1st step of velocity verlet.
 * @param[in]     accel     Initial acceleration
 * @param[in]     deltat    Time step
 * @param[in]     size      State size
 * @param[out]    dtheta    Position delta, in velocity space
 * @param[in,out] velocity  Updated velocity vector
 * @param[out]    init_vel  Saved initial velocity vector
 * @param[out]    init_acc  Saved initial acceleration vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
generalized_step_velocity_verlet_step_one (
   double const * ER7_UTILS_RESTRICT accel,
   double deltat,
   int size,
   double * ER7_UTILS_RESTRICT dtheta,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT init_vel,
   double * ER7_UTILS_RESTRICT init_acc)
{
   double hdt = 0.5*deltat;

   for (int ii = 0; ii < size; ++ii) {
      double hdv = accel[ii]*hdt;
      double vmid = velocity[ii] + hdv;
      init_acc[ii]  = accel[ii];
      init_vel[ii]  = velocity[ii];
      velocity[ii]  = vmid + hdv;
      dtheta[ii]    = vmid * deltat;
   }
}


/**
 * Advance velocity for the 2nd step of velocity verlet.
 * @param[in]     init_vel  Initial velocity
 * @param[in]     init_acc  Initial acceleration
 * @param[in]     end_acc   Final acceleration
 * @param[in]     deltat    Time step
 * @param[in]     size      State size
 * @param[out]    velocity  Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
velocity_verlet_step_two (
   double const * ER7_UTILS_RESTRICT init_vel,
   double const * ER7_UTILS_RESTRICT init_acc,
   double const * ER7_UTILS_RESTRICT end_acc,
   double deltat,
   int size,
   double * ER7_UTILS_RESTRICT velocity)
{
   double hdt = 0.5*deltat;

   for (int ii = 0; ii < size; ++ii) {
      velocity[ii]  = init_vel[ii] + (init_acc[ii]+end_acc[ii])*hdt;
   }
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
VelocityVerletSimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   // Advance state per velocity verlet.
   switch (target_stage) {
   case 1:
      simple_velocity_verlet_step_one (
         accel, dyn_dt, state_size[1],
         position, velocity, init_vel, init_acc);
      break;

   case 2:
      velocity_verlet_step_two (
         init_vel, init_acc, accel, dyn_dt, state_size[1], velocity);
      break;
   }

   return 1.0;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
VelocityVerletGeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{

   // Advance state per velocity verlet.
   switch (target_stage) {
   case 1:
      generalized_deriv_velocity_verlet_step_one (
         accel, dyn_dt, state_size[1],
         dtheta, velocity, init_vel, init_acc);
      compute_posdot (position, dtheta, posdot);
      integ_utils::inplace_euler_step (
         posdot, dyn_dt, state_size[0], position);
      break;

   case 2:
      velocity_verlet_step_two (
         init_vel, init_acc, accel, dyn_dt, state_size[1], velocity);
      break;
   }

   return 1.0;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
VelocityVerletGeneralizedStepSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{

   // Advance state per velocity verlet.
   switch (target_stage) {
   case 1:
      generalized_step_velocity_verlet_step_one (
         accel, dyn_dt, state_size[1],
         dtheta, velocity, init_vel, init_acc);
      compute_expmap_position_step (position, dtheta, position);
      break;

   case 2:
      velocity_verlet_step_two (
         init_vel, init_acc, accel, dyn_dt, state_size[1], velocity);
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

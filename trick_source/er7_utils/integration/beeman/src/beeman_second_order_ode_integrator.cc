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
 * Defines member functions for the class BeemanSecondOrderODEIntegrator.
 */

/*
Purpose: ()
*/

// System includes

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/abm_utils.hh"
#include "er7_utils/integration/core/include/integ_utils.hh"
#include "er7_utils/integration/core/include/integrator_constructor.hh"
#include "er7_utils/integration/core/include/integration_controls.hh"

// Model includes
#include "../include/beeman_second_order_ode_integrator.hh"


namespace er7_utils {



//BeemanSecondOrderODEIntegrator default constructor.
BeemanSecondOrderODEIntegrator::BeemanSecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   PrimingSecondOrderODEIntegrator (),
   init_vel (NULL),
   mean_vel (NULL),
   init_acc (NULL),
   prev_acc (NULL),
   posdot (NULL),
   posddot (NULL),
   init_posddot (NULL),
   prev_posddot (NULL)
{
}


//BeemanSecondOrderODEIntegrator copy constructor.
BeemanSecondOrderODEIntegrator::BeemanSecondOrderODEIntegrator (
   const BeemanSecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   PrimingSecondOrderODEIntegrator (src),
   init_vel (NULL),
   mean_vel (NULL),
   init_acc (NULL),
   prev_acc (NULL),
   posdot (NULL),
   posddot (NULL),
   init_posddot (NULL),
   prev_posddot (NULL)
{
   // Replicate the source's contents if they exist.
   if (problem_type == Integration::GeneralizedDerivSecondOrderODE) {
       init_vel = alloc::replicate_array (state_size[1], src.init_vel);
       mean_vel = alloc::replicate_array (state_size[1], src.mean_vel);
       init_acc = alloc::replicate_array (state_size[1], src.init_acc);
       prev_acc = alloc::replicate_array (state_size[1], src.prev_acc);
       posdot       = alloc::replicate_array (state_size[0], src.posdot);
       posddot      = alloc::replicate_array (state_size[0], src.posddot);
       init_posddot = alloc::replicate_array (state_size[0], src.init_posddot);
       prev_posddot = alloc::replicate_array (state_size[0], src.prev_posddot);
   }

   else if (src.init_vel != NULL) {
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      mean_vel = alloc::replicate_array (state_size[1], src.mean_vel);
      init_acc = alloc::replicate_array (state_size[1], src.init_acc);
      prev_acc = alloc::replicate_array (state_size[1], src.prev_acc);
   }
}



// BeemanSecondOrderODEIntegrator non-default constructor for a simple
// second order ODE.
BeemanSecondOrderODEIntegrator::BeemanSecondOrderODEIntegrator (
   const IntegratorConstructor & primer_constructor,
   unsigned int size,
   IntegrationControls & controls_in)
:
   Er7UtilsDeletable (),
   PrimingSecondOrderODEIntegrator (2, primer_constructor, size, controls_in),
   init_vel (NULL),
   mean_vel (NULL),
   init_acc (NULL),
   prev_acc (NULL),
   posdot (NULL),
   posddot (NULL),
   init_posddot (NULL),
   prev_posddot (NULL)
{
   // Allocate memory used by Beeman's method.
   init_vel = alloc::allocate_array (size);
   mean_vel = alloc::allocate_array (size);
   init_acc = alloc::allocate_array (size);
   prev_acc = alloc::allocate_array (size);
}


// BeemanSecondOrderODEIntegrator non-default constructor for a generalized
// second order ODE in which position is advanced internally using the position
// derivative computed by the provided derivative function.
BeemanSecondOrderODEIntegrator::BeemanSecondOrderODEIntegrator (
   const IntegratorConstructor & primer_constructor,
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls_in)
:
   Er7UtilsDeletable (),
   PrimingSecondOrderODEIntegrator (2, primer_constructor,
                                    position_size, velocity_size,
                                    deriv_funs, controls_in),
   init_vel (NULL),
   mean_vel (NULL),
   init_acc (NULL),
   prev_acc (NULL),
   posdot (NULL),
   posddot (NULL),
   init_posddot (NULL),
   prev_posddot (NULL)
{
   // Allocate memory used by Beeman's method.
   init_vel = alloc::allocate_array (velocity_size);
   mean_vel = alloc::allocate_array (velocity_size);
   init_acc = alloc::allocate_array (velocity_size);
   prev_acc = alloc::allocate_array (velocity_size);
   posdot       = alloc::allocate_array (position_size);
   posddot      = alloc::allocate_array (position_size);
   init_posddot = alloc::allocate_array (position_size);
   prev_posddot = alloc::allocate_array (position_size);
}


// BeemanSecondOrderODEIntegrator destructor.
BeemanSecondOrderODEIntegrator::~BeemanSecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array (init_vel);
   alloc::deallocate_array (mean_vel);
   alloc::deallocate_array (init_acc);
   alloc::deallocate_array (prev_acc);
   alloc::deallocate_array (posdot);
   alloc::deallocate_array (posddot);
   alloc::deallocate_array (init_posddot);
   alloc::deallocate_array (prev_posddot);
}


// Non-throwing swap.
void
BeemanSecondOrderODEIntegrator::swap (
   BeemanSecondOrderODEIntegrator & other)
{
   PrimingSecondOrderODEIntegrator::swap (other);

   std::swap (init_vel, other.init_vel);
   std::swap (mean_vel, other.mean_vel);
   std::swap (init_acc, other.init_acc);
   std::swap (prev_acc, other.prev_acc);
   std::swap (posdot, other.posdot);
   std::swap (posddot, other.posddot);
   std::swap (init_posddot, other.init_posddot);
   std::swap (prev_posddot, other.prev_posddot);
}


// Clone a BeemanSimpleSecondOrderODEIntegrator.
BeemanSimpleSecondOrderODEIntegrator *
BeemanSimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a BeemanGeneralizedDerivSecondOrderODEIntegrator.
BeemanGeneralizedDerivSecondOrderODEIntegrator *
BeemanGeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Save derivatives during priming
void
BeemanSimpleSecondOrderODEIntegrator::technique_save_derivatives (
   int,
   double const * ER7_UTILS_RESTRICT accel,
   double const *,
   double const *)
{
   integ_utils::copy_array (accel, state_size[1], init_acc);
}


// Save derivatives during priming
void
BeemanGeneralizedDerivSecondOrderODEIntegrator::technique_save_derivatives (
   int,
   double const * ER7_UTILS_RESTRICT accel,
   double const * ER7_UTILS_RESTRICT velocity,
   double const * ER7_UTILS_RESTRICT position)
{
   integ_utils::copy_array (accel, state_size[1], init_acc);
   compute_posdotdot (position, velocity, accel, init_posddot);
}


/**
 * Advance position and velocity for the 1st step of Beeman's method.
 * @param[in]     accel       Current acceleration
 * @param[in]     prev_acc    Acceleration at start of previous integ cycle
 * @param[in]     deltat      Time step
 * @param[in]     size        State size
 * @param[out]    init_vel    Saved velocity vector
 * @param[out]    init_acc    Saved acceleration vector
 * @param[in,out] position    Updated position vector
 * @param[in,out] velocity    Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
simple_beeman_step_one (
   double const * ER7_UTILS_RESTRICT accel,
   double const * ER7_UTILS_RESTRICT prev_acc,
   double deltat,
   int size,
   double * ER7_UTILS_RESTRICT init_vel,
   double * ER7_UTILS_RESTRICT init_acc,
   double * ER7_UTILS_RESTRICT position,
   double * ER7_UTILS_RESTRICT velocity)
{
   static const double one_sixth = 1.0 / 6.0;
   double dto6 = one_sixth * deltat;

   for (int ii = 0; ii < size; ++ii) {
      double acc_prev = prev_acc[ii];
      double acc = accel[ii];
      double vel = velocity[ii];
      double vdot = (3.0*acc - acc_prev) * 0.5;
      double xdot = vel + (4.0*acc - acc_prev) * dto6;
      init_vel[ii]  = vel;
      init_acc[ii]  = acc;
      velocity[ii] += vdot*deltat;
      position[ii] += xdot*deltat;
   }
}


/**
 * Advance velocity for the 1st step of Beeman's method.
 * @param[in]     accel       Current acceleration
 * @param[in]     prev_acc    Acceleration at start of previous integ cycle
 * @param[in]     deltat      Time step
 * @param[in]     size        State size
 * @param[out]    init_vel    Saved velocity vector
 * @param[out]    init_acc    Saved acceleration vector
 * @param[in,out] velocity    Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
generalized_beeman_velocity_step_one (
   double const * ER7_UTILS_RESTRICT accel,
   double const * ER7_UTILS_RESTRICT prev_acc,
   double deltat,
   int size,
   double * ER7_UTILS_RESTRICT init_vel,
   double * ER7_UTILS_RESTRICT init_acc,
   double * ER7_UTILS_RESTRICT velocity)
{
   for (int ii = 0; ii < size; ++ii) {
      double acc_prev = prev_acc[ii];
      double acc = accel[ii];
      double vel = velocity[ii];
      double vdot = (3.0*acc - acc_prev) * 0.5;
      init_vel[ii]  = vel;
      init_acc[ii]  = acc;
      velocity[ii] += vdot*deltat;
   }
}


/**
 * Advance position for the 1st step of Beeman's method.
 * @param[in]     init_posddot  Current position 2nd deriv
 * @param[in]     prev_posddot  Posn 2nd deriv at start of previous integ cycle
 * @param[in]     deltat        Time step
 * @param[in]     size          State size
 * @param[in,out] posdot        Position derivative (tweaked on output)
 */
inline void ER7_UTILS_ALWAYS_INLINE
generalized_beeman_position_step_one (
   double const * ER7_UTILS_RESTRICT init_posddot,
   double const * ER7_UTILS_RESTRICT prev_posddot,
   double deltat,
   int size,
   double * ER7_UTILS_RESTRICT posdot)
{
   static const double one_sixth = 1.0 / 6.0;
   double dto6 = one_sixth * deltat;

   for (int ii = 0; ii < size; ++ii) {
      posdot[ii] += (4.0*init_posddot[ii] - prev_posddot[ii]) * dto6;
   }
}


/**
 * Advance velocity for the 2nd step of Beeman's method.
 * @param[in]     init_vel    Saved velocity vector
 * @param[in]     accel       Current acceleration
 * @param[in]     prev_acc    Acceleration at start of previous integ cycle
 * @param[in]     init_acc    Acceleration at start of current integ cycle
 * @param[in]     deltat      Time step
 * @param[in]     size        State size
 * @param[out]    velocity    Updated velocity vector
 */
inline void ER7_UTILS_ALWAYS_INLINE
beeman_step_two (
   double const * ER7_UTILS_RESTRICT init_vel,
   double const * ER7_UTILS_RESTRICT accel,
   double const * ER7_UTILS_RESTRICT prev_acc,
   double const * ER7_UTILS_RESTRICT init_acc,
   double deltat,
   int size,
   double * ER7_UTILS_RESTRICT velocity)
{
   static const double one_sixth = 1.0 / 6.0;
   double dto6 = one_sixth * deltat;

   for (int ii = 0; ii < size; ++ii) {
      velocity[ii] = init_vel[ii] +
                     (2.0*accel[ii] + 5.0*init_acc[ii] - prev_acc[ii]) * dto6;
   }
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
BeemanSimpleSecondOrderODEIntegrator::technique_integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   // Primed: Integrate state via Beeman's method
   switch (target_stage) {
   // Target stage 1 represents the Beeman's method predictor stage.
   case 1:
      // Rotate the history.
      std::swap (init_acc, prev_acc);

      // Perform the predictor step.
      simple_beeman_step_one (
         accel, prev_acc, dyn_dt, state_size[0],
         init_vel, init_acc, position, velocity);
      break;

   // Target stage 2 represents the Beeman's method corrector stage.
   case 2:
      beeman_step_two (
         init_vel, accel, prev_acc, init_acc, dyn_dt, state_size[0], velocity);
      break;
   }

   return 1.0;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
BeemanGeneralizedDerivSecondOrderODEIntegrator::technique_integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   switch (target_stage) {
   // Target stage 1 represents the Beeman's method predictor stage.
   case 1:
      // Rotate the history.
      std::swap (init_acc, prev_acc);
      std::swap (init_posddot, prev_posddot);

      // Compute the position derivatives.
      compute_posdot (position, velocity, posdot);
      compute_posdotdot (position, velocity, accel, init_posddot);

      // Modify the position derivative per the Beeman position predictor.
      generalized_beeman_position_step_one (
         init_posddot, prev_posddot, dyn_dt, state_size[0], posdot);

      // Advance position as an Euler step with the updated position derivative.
      integ_utils::euler_step (
         position, posdot, dyn_dt, state_size[0], position);

      // Advance velocity per the Beeman velocity predictor step.
      generalized_beeman_velocity_step_one (
         accel, prev_acc, dyn_dt, state_size[1],
         init_vel, init_acc, velocity);
      break;

   // Target stage 2 represents the Beeman's method corrector stage.
   // Velocity is corrected, but not position.
   case 2:
      beeman_step_two (
         init_vel, accel, prev_acc, init_acc, dyn_dt, state_size[1], velocity);
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

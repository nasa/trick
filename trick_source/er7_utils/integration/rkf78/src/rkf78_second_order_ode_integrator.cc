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
 * Defines member functions for the class RKFehlberg78SecondOrderODEIntegrator.
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

// Local includes
#include "../include/rkf78_butcher_tableau.hh"
#include "../include/rkf78_second_order_ode_integrator.hh"


/**
 * @internal
 *
 * @def RKF78_BUTCHER_TABLEAU
 * Identifies which of the two Runge Kutta Fehlberg 7/8 Butcher tableaus
 * is to be used based on whether RKFEHLBERG78_USE_STEP_10 is defined.
 */
#ifdef RKFEHLBERG78_USE_STEP_10
#define RKF78_BUTCHER_TABLEAU RKFehlberg78ButcherTableau
#else
#define RKF78_BUTCHER_TABLEAU RKFehlberg78AbbreviatedButcherTableau
#endif


namespace er7_utils {


/**
 * Make a simple Runge Kutta Fehlberg 7/8 intermediate step.
 * @tparam        target_stage  The stage of the integration process
 *                              that the integrator should try to attain.
 * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
 * @param[in]     state_size    The size of the state vectors.
 * @param[in]     init_pos      Initial position.
 * @param[in]     init_vel      Initial velocity.
 * @param[in]     accel         Current acceleration.
 * @param[in,out] veldot_hist   Velocity derivative (acceleration) history.
 * @param[in,out] posdot_hist   Position derivative (velocity) history.
 * @param[in,out] velocity      Generalized velocity vector.
 * @param[in,out] position      Generalized position vector.
 *
 * @return Fractional amount by which time should be advanced.
 */
template <int target_stage>
inline double ER7_UTILS_ALWAYS_INLINE
rkf78_simple_intermediate_step (
   double dyn_dt,
   int state_size[2],
   double const * ER7_UTILS_RESTRICT init_pos,
   double const * ER7_UTILS_RESTRICT init_vel,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT veldot_hist,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT posdot_hist,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   // Advance position and velocity per the 'a' element of the Butcher tableau,
   // saving velocity and acceleration in the derivatives history buffers.
   integ_utils::two_state_weighted_step_save_derivs<target_stage> (
      init_pos, init_vel, accel,
      RKF78_BUTCHER_TABLEAU::RKa[target_stage], dyn_dt, state_size[0],
      posdot_hist, veldot_hist, position, velocity);

   // Advance time per the 'c' element of the Butcher tableau.
   return RKF78_BUTCHER_TABLEAU::RKc[target_stage];
}


/**
 * Make a generalized deriv Runge Kutta Fehlberg 7/8 intermediate step.
 * @tparam        target_stage  The stage of the integration process
 *                              that the integrator should try to attain.
 * @param[in]     deriv_fun     Function that computes position derivative.
 * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
 * @param[in]     state_size    The size of the state vectors.
 * @param[in]     init_pos      Initial position.
 * @param[in]     init_vel      Initial velocity.
 * @param[in]     accel         Current acceleration.
 * @param[in,out] veldot_hist   Velocity derivative (acceleration) history.
 * @param[in,out] posdot_hist   Position derivative (velocity) history.
 * @param[in,out] velocity      Generalized velocity vector.
 * @param[in,out] position      Generalized position vector.
 *
 * @return Fractional amount by which time should be advanced.
 */
template <int target_stage>
inline double ER7_UTILS_ALWAYS_INLINE
rkf78_generalized_deriv_intermediate_step (
   GeneralizedPositionDerivativeFunctions::FirstDerivative deriv_fun,
   double dyn_dt,
   int state_size[2],
   double const * ER7_UTILS_RESTRICT init_pos,
   double const * ER7_UTILS_RESTRICT init_vel,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT veldot_hist,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT posdot_hist,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   // Compute time derivative of canonical position,
   // saving the result in the position derivative history.
   deriv_fun (position, velocity, posdot_hist[target_stage-1]);

   // Advance generalized position and velocity.
   integ_utils::two_state_weighted_step_save_veldot<target_stage> (
      init_pos, init_vel, accel,
      RKF78_BUTCHER_TABLEAU::RKa[target_stage], dyn_dt, state_size,
      posdot_hist, veldot_hist, position, velocity);

   // Advance time per the 'c' element of the Butcher tableau.
   return RKF78_BUTCHER_TABLEAU::RKc[target_stage];
}


/**
 * Make a generalized step Runge Kutta Fehlberg 7/8 intermediate step.
 * @tparam        target_stage  The stage of the integration process
 *                              that the integrator should try to attain.
 * @param[in]     xform_fun     Function that transforms generalized velocity.
 * @param[in]     step_fun      Function that advances generalized position.
 * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
 * @param[in]     vel_size      The size of the generalized velocity.
 * @param[in]     init_pos      Initial position.
 * @param[in]     init_vel      Initial velocity.
 * @param[in]     accel         Current acceleration.
 * @param[in,out] veldot_hist   Velocity derivative (acceleration) history.
 * @param[in,out] posdot_hist   Position derivative (velocity) history.
 * @param[in,out] dtheta        Position step, in the velocity space.
 * @param[in,out] velocity      Generalized velocity vector.
 * @param[in,out] position      Generalized position vector.
 *
 * @return Fractional amount by which time should be advanced.
 */
template <int target_stage>
inline double ER7_UTILS_ALWAYS_INLINE
rkf78_generalized_step_intermediate_step (
   GeneralizedPositionStepFunctions::DexpinvTransformVelocity xform_fun,
   GeneralizedPositionStepFunctions::ExpMapPositionStep step_fun,
   double dyn_dt,
   int vel_size,
   double const * ER7_UTILS_RESTRICT init_pos,
   double const * ER7_UTILS_RESTRICT init_vel,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT veldot_hist,
   double * ER7_UTILS_RESTRICT * ER7_UTILS_RESTRICT posdot_hist,
   double * ER7_UTILS_RESTRICT dtheta,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   // Transform velocity to the start of the interval,
   // saving the result in the position derivative history.
   xform_fun (velocity, dtheta, posdot_hist[target_stage-1]);

   // Compute the position step for this stage.
   integ_utils::weighted_sum<target_stage> (
      posdot_hist, RKF78_BUTCHER_TABLEAU::RKa[target_stage], dyn_dt, vel_size,
      dtheta);

   // Take the non-linear position step.
   step_fun (init_pos, dtheta, position);

   // Make a weighted step for velocity based on the RKF78 Butcher tableau,
   // saving the input acceleration in veldot_hist.
   integ_utils::weighted_step_save_deriv<target_stage> (
      init_vel, accel,
      RKF78_BUTCHER_TABLEAU::RKa[target_stage], dyn_dt, vel_size,
      veldot_hist, velocity);

   // Advance time per the 'c' element of the Butcher tableau.
   return RKF78_BUTCHER_TABLEAU::RKc[target_stage];
}



// Default constructor for an RKFehlberg78SecondOrderODEIntegrator.
RKFehlberg78SecondOrderODEIntegrator::RKFehlberg78SecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (),
   init_pos (NULL),
   init_vel (NULL),
   dtheta (NULL)
{
   // Initialize RKF78 memory to NULL pointers.
   alloc::initialize_2D_array<13> (posdot_hist);
   alloc::initialize_2D_array<13> (veldot_hist);
}


// Copy constructor for an RKFehlberg78SecondOrderODEIntegrator.
RKFehlberg78SecondOrderODEIntegrator::RKFehlberg78SecondOrderODEIntegrator (
   const RKFehlberg78SecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (src),
   init_pos (NULL),
   init_vel (NULL),
   dtheta (NULL)
{
   if (problem_type == Integration::GeneralizedStepSecondOrderODE) {
      init_pos     = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel     = alloc::replicate_array (state_size[1], src.init_vel);
      dtheta       = alloc::replicate_array (state_size[1], src.dtheta);
      alloc::replicate_2D_array<13> (state_size[1], src.posdot_hist,
                                     posdot_hist);
      alloc::replicate_2D_array<13> (state_size[1], src.veldot_hist,
                                     veldot_hist);

   }

   else if (src.init_pos != NULL) {
      init_pos     = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel     = alloc::replicate_array (state_size[1], src.init_vel);
      alloc::replicate_2D_array<13> (state_size[0], src.posdot_hist,
                                     posdot_hist);
      alloc::replicate_2D_array<13> (state_size[1], src.veldot_hist,
                                     veldot_hist);
   }

   else {
      alloc::initialize_2D_array<13> (posdot_hist);
      alloc::initialize_2D_array<13> (veldot_hist);
   }
}


// Non-default constructor for an RKFehlberg78SecondOrderODEIntegrator
// in which the time derivative of position is the generalized velocity.
RKFehlberg78SecondOrderODEIntegrator::RKFehlberg78SecondOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   SecondOrderODEIntegrator (size, controls),
   init_pos (NULL),
   init_vel (NULL),
   dtheta (NULL)
{
   // Allocate memory used by eighth order Runge Kutta Fehlberg algorithm.
   init_pos = alloc::allocate_array (size);
   init_vel = alloc::allocate_array (size);
   alloc::allocate_2D_array<13> (size, posdot_hist);
   alloc::allocate_2D_array<13> (size, veldot_hist);
}


// Non-default constructor for an RKFehlberg78SecondOrderODEIntegrator
// for generalized position, generalized velocity.
RKFehlberg78SecondOrderODEIntegrator::RKFehlberg78SecondOrderODEIntegrator (
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
   dtheta (NULL)
{
   // Allocate memory used by eighth order Runge Kutta Fehlberg algorithm.
   init_pos = alloc::allocate_array (position_size);
   init_vel = alloc::allocate_array (velocity_size);
   alloc::allocate_2D_array<13> (position_size, posdot_hist);
   alloc::allocate_2D_array<13> (velocity_size, veldot_hist);
}


// Non-default constructor for an RKFehlberg78SecondOrderODEIntegrator
// for generalized position, generalized velocity.
RKFehlberg78SecondOrderODEIntegrator::RKFehlberg78SecondOrderODEIntegrator (
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
   dtheta (NULL)
{
   // Allocate memory used by eighth order Runge Kutta Fehlberg algorithm.
   init_pos = alloc::allocate_array (position_size);
   init_vel = alloc::allocate_array (velocity_size);
   dtheta = alloc::allocate_array (velocity_size);
   alloc::allocate_2D_array<13> (velocity_size, posdot_hist);
   alloc::allocate_2D_array<13> (velocity_size, veldot_hist);
}


// RKFehlberg78SecondOrderODEIntegrator destructor.
RKFehlberg78SecondOrderODEIntegrator::~RKFehlberg78SecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array (init_pos);
   alloc::deallocate_array (init_vel);
   alloc::deallocate_array (dtheta);
   alloc::deallocate_2D_array<13> (posdot_hist);
   alloc::deallocate_2D_array<13> (veldot_hist);
}


// Non-throwing swap.
void
RKFehlberg78SecondOrderODEIntegrator::swap (
   RKFehlberg78SecondOrderODEIntegrator & other)
{
   SecondOrderODEIntegrator::swap (other);

   std::swap (init_pos, other.init_pos);
   std::swap (init_vel, other.init_vel);
   std::swap (dtheta, other.dtheta);
   for (int ii = 0; ii < 13; ++ii) {
      std::swap (posdot_hist[ii], other.posdot_hist[ii]);
      std::swap (veldot_hist[ii], other.veldot_hist[ii]);
   }
}


// Clone a RKFehlberg78SimpleSecondOrderODEIntegrator.
RKFehlberg78SimpleSecondOrderODEIntegrator *
RKFehlberg78SimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator.
RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator *
RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a RKFehlberg78GeneralizedStepSecondOrderODEIntegrator.
RKFehlberg78GeneralizedStepSecondOrderODEIntegrator *
RKFehlberg78GeneralizedStepSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Propagate state for the special case of velocity being the derivative of
// position.
IntegratorResult
RKFehlberg78SimpleSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {

   // Initial stage (stage 1):
   // Save initial state and update per RKF78 RKa[1] (one element).
   case 1:
      integ_utils::inplace_two_state_euler_step_save_all (
         accel,
         RKF78_BUTCHER_TABLEAU::RKa[1][0]*dyn_dt,
         state_size[0],
         init_pos, init_vel,
         posdot_hist[0], veldot_hist[0],
         position, velocity);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[1];
      break;

   // Intermediate stages (2 to 12):
   // Update state per RKF78 RKa[target_stage] (target_stage elements).
   case 2:
      step_factor =
         rkf78_simple_intermediate_step<2> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   case 3:
      step_factor =
         rkf78_simple_intermediate_step<3> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   case 4:
      step_factor =
         rkf78_simple_intermediate_step<4> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   case 5:
      step_factor =
         rkf78_simple_intermediate_step<5> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   case 6:
      step_factor =
         rkf78_simple_intermediate_step<6> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   case 7:
      step_factor =
         rkf78_simple_intermediate_step<7> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   case 8:
      step_factor =
         rkf78_simple_intermediate_step<8> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   case 9:
      step_factor =
         rkf78_simple_intermediate_step<9> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   case 10:
      step_factor =
         rkf78_simple_intermediate_step<10> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   case 11:
      step_factor =
         rkf78_simple_intermediate_step<11> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   case 12:
      step_factor =
         rkf78_simple_intermediate_step<12> (
            dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
   break;

   // Final stage (13):
   // Update state per RKF78 RKb5 (13 elements).
   case 13:
      integ_utils::two_state_weighted_step<13> (
         init_pos, init_vel, accel, posdot_hist, veldot_hist,
         RKF78_BUTCHER_TABLEAU::RKb8, dyn_dt, state_size[0],
         position, velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {

   // Initial stage (stage 1):
   // Save initial state and update per RKF78 RKa[1] (one element).
   case 1:

      // Compute time derivative of canonical position,
      // saving the result in the position derivative history.
      compute_posdot (position, velocity, posdot_hist[0]);

      // Advance position and velocity.
      integ_utils::inplace_two_state_euler_step_save_pos_vel_acc (
         posdot_hist[0], accel,
         RKF78_BUTCHER_TABLEAU::RKa[1][0]*dyn_dt, state_size,
         init_pos, init_vel, veldot_hist[0],
         position, velocity);

      step_factor = RKF78_BUTCHER_TABLEAU::RKc[1];
      break;

   // Intermediate stages (2 to 12):
   // Update state per RKF78 RKa[target_stage] (target_stage elements).
   case 2:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<2> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   case 3:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<3> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   case 4:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<4> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   case 5:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<5> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   case 6:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<6> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   case 7:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<7> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   case 8:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<8> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   case 9:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<9> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   case 10:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<10> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   case 11:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<11> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   case 12:
      step_factor =
         rkf78_generalized_deriv_intermediate_step<12> (
            compute_posdot, dyn_dt, state_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, velocity, position);
      break;

   // Final stage (13):
   // Update state per RKF78 RKb8 (13 elements).
   case 13:
      compute_posdot (position, velocity, posdot_hist[12]);

      integ_utils::two_state_weighted_step<13> (
         init_pos, init_vel, posdot_hist[12], accel, posdot_hist, veldot_hist,
         RKF78_BUTCHER_TABLEAU::RKb8, dyn_dt, state_size,
         position, velocity);
      step_factor = 1.0;
      break;

   default:
      step_factor = 1.0;
      break;
   }

   return step_factor;
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
RKFehlberg78GeneralizedStepSecondOrderODEIntegrator::integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;
   int vel_size = state_size[1];

   // Advance the state.
   // Integrate to the next stage
   switch (target_stage) {

   // Initial stage (stage 1):
   // Save initial state and update per RKF78 initial step.
   case 1: {
      double dt = RKF78_BUTCHER_TABLEAU::RKc[1]*dyn_dt;

      // Save the initial position and velocity in init_pos and posdot_hist.
      integ_utils::two_state_copy_array (
         position, velocity, state_size, init_pos, posdot_hist[0]);

      // Compute the initial position step.
      integ_utils::scale_array (velocity, dt, state_size[1], dtheta);

      // Make a non-linear position step.
      compute_expmap_position_step (init_pos, dtheta, position);

      // Make an in-place Euler step for velocity to the stage 1 end time,
      // saving the initial velocity in init_vel, acceleration in veldot_hist.
      integ_utils::inplace_euler_step_save_both (
         accel, dt, vel_size,
         init_vel, veldot_hist[0], velocity);

      // Advance time to stage 1 end time.
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[1];
      break;
   }

   // Intermediate stages (2 to 12):
   // Update state per RKF78 RKa[target_stage] (target_stage elements).
   case 2:
      step_factor =
         rkf78_generalized_step_intermediate_step<2> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   case 3:
      step_factor =
         rkf78_generalized_step_intermediate_step<3> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   case 4:
      step_factor =
         rkf78_generalized_step_intermediate_step<4> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   case 5:
      step_factor =
         rkf78_generalized_step_intermediate_step<5> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   case 6:
      step_factor =
         rkf78_generalized_step_intermediate_step<6> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   case 7:
      step_factor =
         rkf78_generalized_step_intermediate_step<7> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   case 8:
      step_factor =
         rkf78_generalized_step_intermediate_step<8> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   case 9:
      step_factor =
         rkf78_generalized_step_intermediate_step<9> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   case 10:
      step_factor =
         rkf78_generalized_step_intermediate_step<10> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   case 11:
      step_factor =
         rkf78_generalized_step_intermediate_step<11> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   case 12:
      step_factor =
         rkf78_generalized_step_intermediate_step<12> (
            compute_dexpinv_velocity_transform, compute_expmap_position_step,
            dyn_dt, vel_size, init_pos, init_vel, accel,
            veldot_hist, posdot_hist, dtheta, velocity, position);
      break;

   // Final stage (13):
   // Update state per RKF78 RKb8 (13 elements).
   case 13:

      // Transform velocity to the start of the interval,
      // saving the result in the position derivative history.
      compute_dexpinv_velocity_transform (velocity, dtheta, posdot_hist[12]);

      // Compute the position step for this stage.
      integ_utils::weighted_sum<13> (
         posdot_hist, RKF78_BUTCHER_TABLEAU::RKb8, dyn_dt, vel_size,
         dtheta);

      // Take the non-linear position step.
      compute_expmap_position_step (init_pos, dtheta, position);

      integ_utils::weighted_step<13> (
          init_vel, accel, veldot_hist,
          RKF78_BUTCHER_TABLEAU::RKb8, dyn_dt, vel_size,
          velocity);

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

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
 * Defines member functions for the class ABM4SecondOrderODEIntegrator.
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
#include "../include/abm4_second_order_ode_integrator.hh"


namespace {

   /**
    * Scale factor on weights so that the weights can be integral.
    */
   const double wscale = 1.0/24.0;

   /**
    * Fourth order Adams Bashforth weights.
    */
   const double predictor_weights[4] = {55.0, -59.0, 37.0, -9.0};

   /**
    * Fourth order Adams Bashforth weights, scaled.
    */
   const double scaled_predictor_weights[4] = {
      55.0/24.0, -59.0/24.0, 37.0/24.0, -9.0/24.0};

   /**
    * Third order Adams Moulton weights.
    */
   const double corrector_weights[4] = { 9.0,  19.0, -5.0,  1.0};

   /**
    * Third order Adams Moulton weights, scaled.
    */
   const double scaled_corrector_weights[4] = {
      9.0/24.0,  19.0/24.0, -5.0/24.0,  1.0/24.0};
}


namespace er7_utils {



// ABM4SecondOrderODEIntegrator default constructor.
ABM4SecondOrderODEIntegrator::ABM4SecondOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   PrimingSecondOrderODEIntegrator (),
   init_pos (NULL),
   init_vel (NULL),
   posdot (NULL),
   posdot_hist_anchor (NULL),
   veldot_hist_anchor (NULL)
#if 0
   dtheta (NULL)
#endif
{
   // Initialize memory used by ABM4 algorithm.
   alloc::initialize_2D_array<double, 4> (posdot_hist);
   posdot_hist_anchor = posdot_hist[0];
   alloc::initialize_2D_array<double, 4> (veldot_hist);
   veldot_hist_anchor = veldot_hist[0];
}


// Copy constructor.
ABM4SecondOrderODEIntegrator::ABM4SecondOrderODEIntegrator (
   const ABM4SecondOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   PrimingSecondOrderODEIntegrator (src),
   init_pos (NULL),
   init_vel (NULL),
   posdot (NULL),
   posdot_hist_anchor (NULL),
   veldot_hist_anchor (NULL)
#if 0
   dtheta (NULL)
#endif
{
   // Replicate the source's contents if they exist.
#if 0
   if (problem_type == Integration::GeneralizedStepSecondOrderODE) {
      // Fill this in if/when this technique is revitalized.
   }
   else
#endif
   if (problem_type == Integration::GeneralizedDerivSecondOrderODE) {
      init_pos = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      posdot = alloc::replicate_array (state_size[0], src.posdot);
      alloc::replicate_2D_array<double, 4> (state_size[0], src.posdot_hist,
                                            posdot_hist);
      posdot_hist_anchor = posdot_hist[0];
      alloc::replicate_2D_array<double, 4> (state_size[1], src.veldot_hist,
                                            veldot_hist);
      veldot_hist_anchor = veldot_hist[0];
   }

   else if (src.init_pos != NULL) {
      init_pos = alloc::replicate_array (state_size[0], src.init_pos);
      init_vel = alloc::replicate_array (state_size[1], src.init_vel);
      alloc::replicate_2D_array<double, 4> (state_size[0], src.posdot_hist,
                                            posdot_hist);
      posdot_hist_anchor = posdot_hist[0];
      alloc::replicate_2D_array<double, 4> (state_size[1], src.veldot_hist,
                                            veldot_hist);
      veldot_hist_anchor = veldot_hist[0];
   }

   else {
      alloc::initialize_2D_array<double, 4> (posdot_hist);
      alloc::initialize_2D_array<double, 4> (veldot_hist);
   }
}



// ABM4SecondOrderODEIntegrator non-default constructor,
// simple second order ODE.
ABM4SecondOrderODEIntegrator::ABM4SecondOrderODEIntegrator (
   const IntegratorConstructor & primer_constructor,
   unsigned int size,
   IntegrationControls & controls_in)
:
   Er7UtilsDeletable (),
   PrimingSecondOrderODEIntegrator (4, primer_constructor, size, controls_in),
   init_pos (NULL),
   init_vel (NULL),
   posdot (NULL),
   posdot_hist_anchor (NULL),
   veldot_hist_anchor (NULL)
#if 0
   dtheta (NULL)
#endif
{
   // Allocate memory used by ABM4.
   init_pos = alloc::allocate_array<double> (size);
   init_vel = alloc::allocate_array<double> (size);
   alloc::allocate_2D_array<double, 4> (size, posdot_hist);
   posdot_hist_anchor = posdot_hist[0];
   alloc::allocate_2D_array<double, 4> (size, veldot_hist);
   veldot_hist_anchor = veldot_hist[0];
}


// ABM4SecondOrderODEIntegrator non-default constructor,
// generalized second order ODE with position advanced internally.
ABM4SecondOrderODEIntegrator::ABM4SecondOrderODEIntegrator (
   const IntegratorConstructor & primer_constructor,
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionDerivativeFunctions & deriv_funs,
   IntegrationControls & controls_in)
:
   Er7UtilsDeletable (),
   PrimingSecondOrderODEIntegrator (4, primer_constructor,
                                    position_size, velocity_size,
                                    deriv_funs, controls_in),
   init_pos (NULL),
   init_vel (NULL),
   posdot (NULL),
   posdot_hist_anchor (NULL),
veldot_hist_anchor (NULL)
#if 0
   dtheta (NULL)
#endif
{
   // Allocate memory used by ABM4.
   init_pos = alloc::allocate_array<double> (position_size);
   init_vel = alloc::allocate_array<double> (velocity_size);
   posdot = alloc::allocate_array<double> (position_size);
   alloc::allocate_2D_array<double, 4> (position_size, posdot_hist);
   posdot_hist_anchor = posdot_hist[0];
   alloc::allocate_2D_array<double, 4> (velocity_size, veldot_hist);
   veldot_hist_anchor = veldot_hist[0];
}


#if 0
// ABM4SecondOrderODEIntegrator non-default constructor,
// generalized second order ODE with position advanced externally.
ABM4SecondOrderODEIntegrator::ABM4SecondOrderODEIntegrator (
   const IntegratorConstructor & primer_constructor,
   unsigned int position_size,
   unsigned int velocity_size,
   const GeneralizedPositionStepFunctions & step_funs,
   IntegrationControls & controls_in)
:
   PrimingSecondOrderODEIntegrator (4, primer_constructor,
                                    position_size, velocity_size,
                                    step_funs, controls_in),
   init_pos (NULL),
   init_vel (NULL),
   posdot (NULL),
   posdot_hist_anchor (NULL),
   veldot_hist_anchor (NULL),
   dtheta (NULL)
{
   // Allocate memory used by ABM4.
   init_pos = alloc::allocate_array<double> (position_size);
   init_vel = alloc::allocate_array<double> (velocity_size);
   posdot = alloc::allocate_array<double> (position_size);
   dtheta = alloc::allocate_array<double> (position_size);
   alloc::allocate_2D_array<double, 4> (position_size, posdot_hist);
   posdot_hist_anchor = posdot_hist[0];
   alloc::allocate_2D_array<double, 4> (velocity_size, veldot_hist);
   veldot_hist_anchor = veldot_hist[0];
}
#endif


// ABM4SecondOrderODEIntegrator destructor.
ABM4SecondOrderODEIntegrator::~ABM4SecondOrderODEIntegrator (
   void)
{
   alloc::deallocate_array<double> (init_pos);
   alloc::deallocate_array<double> (init_vel);
   alloc::deallocate_array<double> (posdot);
#if 0
   alloc::deallocate_array<double> (dtheta);
#endif
   // Restore the cached pointers so that the arrays can be deleted properly.
   posdot_hist[0] = posdot_hist_anchor;
   veldot_hist[0] = veldot_hist_anchor;
   alloc::deallocate_2D_array<double, 4> (posdot_hist);
   alloc::deallocate_2D_array<double, 4> (veldot_hist);
}


// Non-throwing swap.
void
ABM4SecondOrderODEIntegrator::swap (
   ABM4SecondOrderODEIntegrator & other)
{
   PrimingSecondOrderODEIntegrator::swap (other);

   std::swap (init_pos, other.init_pos);
   std::swap (init_vel, other.init_vel);
   std::swap (posdot, other.posdot);
#if 0
   std::swap (dtheta, other.dtheta);
#endif
   for (int ii = 0; ii < 4; ++ii) {
      std::swap (posdot_hist[ii], other.posdot_hist[ii]);
      std::swap (veldot_hist[ii], other.veldot_hist[ii]);
   }
      std::swap (posdot_hist_anchor, other.posdot_hist_anchor);
      std::swap (veldot_hist_anchor, other.veldot_hist_anchor);
}


// Clone a ABM4SimpleSecondOrderODEIntegrator.
ABM4SimpleSecondOrderODEIntegrator *
ABM4SimpleSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Clone a ABM4GeneralizedDerivSecondOrderODEIntegrator.
ABM4GeneralizedDerivSecondOrderODEIntegrator *
ABM4GeneralizedDerivSecondOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Save derivatives during priming
void
ABM4SimpleSecondOrderODEIntegrator::technique_save_derivatives (
   int,
   double const * ER7_UTILS_RESTRICT accel,
   double const * ER7_UTILS_RESTRICT velocity,
   double const *)
{
   abm::rotate_history<4> (posdot_hist, veldot_hist);
   integ_utils::two_state_copy_array (
      velocity, accel, state_size[0],
      posdot_hist[0], veldot_hist[0]);
}


// Propagate state via ABM4, simple second order ODE.
IntegratorResult
ABM4SimpleSecondOrderODEIntegrator::technique_integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   switch (target_stage) {

   // Target stage 1 represents the ABM4 predictor stage.
   // Save derivatives in the circular derivative buffer and advance state
   // propagate state to the end of the integration cycle using the
   // fourth order Adams-Bashforth method as a predictor.
   case 1:
      // Rotate the history so that what was the oldest set of saved
      // derivatives will be overwritten with the incoming derivatives.
      abm::rotate_history<4> (posdot_hist, veldot_hist);

      // Perform the predictor step, saving derivatives.
      abm::predictor_step<4> (accel,
                              predictor_weights, wscale,
                              dyn_dt, state_size[0],
                              posdot_hist, veldot_hist,
                              init_pos, init_vel,
                              position, velocity);
      break;

   // Target stage 2 represents the ABM4 corrector stage.
   // Advance state using third order Adams-Moulton method as a corrector.
   case 2:
      // Perform the corrector step.
      abm::corrector_step<4> (init_pos, init_vel, accel,
                              posdot_hist, veldot_hist,
                              corrector_weights, wscale,
                              dyn_dt, state_size[0],
                              position, velocity);
      break;
   }

   return 1.0;
}


// Save derivatives during priming
void
ABM4GeneralizedDerivSecondOrderODEIntegrator::technique_save_derivatives (
   int,
   double const * ER7_UTILS_RESTRICT accel,
   double const * ER7_UTILS_RESTRICT velocity,
   double const * ER7_UTILS_RESTRICT position)
{
   abm::rotate_history<4> (posdot_hist, veldot_hist);

   // Compute time derivative of canonical position,
   // saving the result in the position derivative history.
   compute_posdot (position, velocity, posdot_hist[0]);

   // Save the acceleration in the velocity derivative history.
   integ_utils::copy_array (accel, state_size[1], veldot_hist[0]);
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
ABM4GeneralizedDerivSecondOrderODEIntegrator::technique_integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   switch (target_stage) {
   // Target stage 1 represents the ABM4 predictor stage.
   // Save derivatives in the circular derivative buffer and advance state
   // propagate state to the end of the integration cycle using the
   // fourth order Adams-Bashforth method as a predictor.
   case 1:
      // Rotate the history.
      abm::rotate_history<4> (posdot_hist, veldot_hist);

      // Compute the time derivative of the generalized position.
      compute_posdot (position, velocity, posdot);

      // Perform the predictor step.
      abm::predictor_step<4> (posdot, accel,
                              predictor_weights, wscale, dyn_dt, state_size,
                              posdot_hist, veldot_hist,
                              init_pos, init_vel,
                              position, velocity);
      break;

   // Target stage 2 represents the ABM4 corrector stage.
   // Advance state using third order Adams-Moulton method as a corrector.
   case 2:
      // Compute the time derivative of the generalized position.
      compute_posdot (position, velocity, posdot);

      // Perform the corrector step.
      abm::corrector_step<4> (init_pos, init_vel, posdot, accel,
                              posdot_hist, veldot_hist,
                              corrector_weights, wscale,
                              dyn_dt, state_size,
                              position, velocity);
      break;
   }

   return 1.0;
}


#if 0
// Save derivatives during priming
void
ABM4GeneralizedStepSecondOrderODEIntegrator::technique_save_derivatives (
   int,
   double const * ER7_UTILS_RESTRICT accel,
   double const * ER7_UTILS_RESTRICT velocity,
   double const * ER7_UTILS_RESTRICT position)
{
   abm::rotate_history<4> (posdot_hist, veldot_hist);
   integ_utils::two_state_copy_array (
      velocity, accel, state_size[1],
      posdot_hist[0], veldot_hist[0]);
}


// Propagate state for the general case of the generalized position derivative
// being a function of generalized position and generalized velocity.
IntegratorResult
ABM4GeneralizedStepSecondOrderODEIntegrator::technique_integrate (
   double dyn_dt,
   unsigned int target_stage,
   double const * ER7_UTILS_RESTRICT accel,
   double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   int vel_size = state_size[1];

   switch (target_stage) {
   // Target stage 1 represents the ABM4 predictor stage.
   // Save derivatives in the circular derivative buffer and advance state
   // propagate state to the end of the integration cycle using the
   // fourth order Adams-Bashforth method as a predictor.
   case 1:
      // Rotate the history.
      abm::rotate_history<4> (posdot_hist, veldot_hist);

      // Save the initial position and current velocity.
      integ_utils::copy_array (position, state_size[0], init_pos);
      integ_utils::copy_array (velocity, vel_size, posdot_hist[0]);

      // Update position via Crouch-Grossman.
      for (int ii = 3; ii >= 0; --ii) {
         integ_utils::scale_array (
            posdot_hist[ii], scaled_predictor_weights[ii]*dyn_dt, vel_size,
            dtheta);
         compute_expmap_position_step (position, dtheta, position);
      }

      // Update velocity.
      abm::predictor_step<4> (accel, predictor_weights,
                              wscale*dyn_dt, vel_size,
                              veldot_hist, init_vel, velocity);
      break;

   // Target stage 2 represents the ABM4 corrector stage.
   // Advance state using third order Adams-Moulton method as a corrector.
   case 2:
      // Update position via Crouch-Grossman.
      integ_utils::scale_array (
         posdot_hist[2], scaled_corrector_weights[3]*dyn_dt, vel_size, dtheta);
      compute_expmap_position_step (init_pos, dtheta, position);
      integ_utils::scale_array (
          posdot_hist[1], scaled_corrector_weights[2]*dyn_dt, vel_size, dtheta);
      compute_expmap_position_step (position, dtheta, position);
      integ_utils::scale_array (
          posdot_hist[0], scaled_corrector_weights[1]*dyn_dt, vel_size, dtheta);
      compute_expmap_position_step (position, dtheta, position);
      integ_utils::scale_array (
          velocity, scaled_corrector_weights[0]*dyn_dt, vel_size, dtheta);
      compute_expmap_position_step (position, dtheta, position);

      // Update velocity.
      abm::corrector_step<4> (init_vel, accel, veldot_hist, corrector_weights,
                              wscale*dyn_dt,  vel_size,
                              velocity);
      break;
   }

   return 1.0;
}
#endif

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

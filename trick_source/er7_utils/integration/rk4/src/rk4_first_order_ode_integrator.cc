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
 * Defines member functions for the class RK4FirstOrderODEIntegrator.
 */

/*
Purpose: ()
*/

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/alloc.hh"

// Integration includes
#include "er7_utils/integration/core/include/integ_utils.hh"
#include "er7_utils/integration/core/include/rk_utils.hh"

// Model includes
#include "../include/rk4_first_order_ode_integrator.hh"


namespace er7_utils {

// RK4FirstOrderODEIntegrator default constructor.
RK4FirstOrderODEIntegrator::RK4FirstOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (),
   init_state (NULL)
{
   alloc::initialize_2D_array<double, 4> (deriv_hist);
}


// RK4FirstOrderODEIntegrator copy constructor.
RK4FirstOrderODEIntegrator::RK4FirstOrderODEIntegrator (
   const RK4FirstOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (src),
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


// RK4FirstOrderODEIntegrator non-default constructor.
RK4FirstOrderODEIntegrator::RK4FirstOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (size, controls),
   init_state (NULL)
{
   // Allocate memory used by Runge Kutta 4 algorithm.
   init_state = alloc::allocate_array<double> (state_size);
   alloc::allocate_2D_array<double, 4> (state_size, deriv_hist);
}


// RK4FirstOrderODEIntegrator destructor.
RK4FirstOrderODEIntegrator::~RK4FirstOrderODEIntegrator (
   void)
{
   // Release memory allocated for use by Runge Kutta 4 algorithm.
   alloc::deallocate_array<double> (init_state);
   alloc::deallocate_2D_array<double, 4> (deriv_hist);
}


// Clone a RK4FirstOrderODEIntegrator.
RK4FirstOrderODEIntegrator *
RK4FirstOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Non-throwing swap.
void
RK4FirstOrderODEIntegrator::swap (
   RK4FirstOrderODEIntegrator & other)
{
   FirstOrderODEIntegrator::swap (other);
   std::swap (init_state, other.init_state);
   std::swap (deriv_hist[0], other.deriv_hist[0]);
   std::swap (deriv_hist[1], other.deriv_hist[1]);
   std::swap (deriv_hist[2], other.deriv_hist[2]);
   std::swap (deriv_hist[3], other.deriv_hist[3]);
}


// Propagate state via the standard RK4 integration scheme.
IntegratorResult
RK4FirstOrderODEIntegrator::integrate (
   double dt,
   unsigned int target_stage,
   const double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   switch (target_stage) {
   case 1:
      integ_utils::inplace_euler_step_save_both (
         velocity, 0.5*dt, state_size,
         init_state, deriv_hist[0], position);
      step_factor = 0.5;
      break;

   case 2:
      integ_utils::euler_step_save_deriv (
         init_state, velocity, 0.5*dt, state_size,
         deriv_hist[1], position);
      step_factor = 0.5;
      break;

   case 3:
      integ_utils::euler_step_save_deriv (
         init_state, velocity, dt, state_size,
         deriv_hist[2], position);
      step_factor = 1.0;
      break;

   case 4:
      rk::rk4_final_step (
         init_state, deriv_hist, velocity, dt, state_size,
         position);
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

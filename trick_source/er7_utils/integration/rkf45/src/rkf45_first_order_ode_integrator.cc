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
 * Defines member functions for the class RKFehlberg45FirstOrderODEIntegrator.
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
#include "../include/rkf45_first_order_ode_integrator.hh"
#include "../include/rkf45_butcher_tableau.hh"


namespace er7_utils {

// RK4FirstOrderODEIntegrator default constructor.
RKFehlberg45FirstOrderODEIntegrator::RKFehlberg45FirstOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (),
   init_state (NULL)
{
   alloc::initialize_2D_array<6> (deriv_hist);
}


// RK4FirstOrderODEIntegrator copy constructor.
RKFehlberg45FirstOrderODEIntegrator::RKFehlberg45FirstOrderODEIntegrator (
   const RKFehlberg45FirstOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (src),
   init_state (NULL)
{
   if (src.init_state != NULL) {
      init_state = alloc::replicate_array (state_size, src.init_state);
      alloc::replicate_2D_array<6> (state_size, src.deriv_hist, deriv_hist);
   }
   else {
      alloc::initialize_2D_array<6> (deriv_hist);
   }
}


// RK4FirstOrderODEIntegrator non-default constructor.
RKFehlberg45FirstOrderODEIntegrator::RKFehlberg45FirstOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (size, controls),
   init_state (NULL)
{

   // Allocate memory used by Runge Kutta Fehlberg 4/5 algorithm.
   init_state = alloc::allocate_array (state_size);
   alloc::allocate_2D_array<6> (state_size, deriv_hist);
}


// RKFehlberg45FirstOrderODEIntegrator destructor.
RKFehlberg45FirstOrderODEIntegrator::~RKFehlberg45FirstOrderODEIntegrator (
   void)
{
   alloc::deallocate_array (init_state);
   alloc::deallocate_2D_array<6> (deriv_hist);
}


// Clone a RKFehlberg45FirstOrderODEIntegrator.
RKFehlberg45FirstOrderODEIntegrator *
RKFehlberg45FirstOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Non-throwing swap.
void
RKFehlberg45FirstOrderODEIntegrator::swap (
   RKFehlberg45FirstOrderODEIntegrator & other)
{
   FirstOrderODEIntegrator::swap (other);

   std::swap (init_state, other.init_state);
   for (int ii = 0; ii < 6; ++ii) {
      std::swap (deriv_hist[ii], other.deriv_hist[ii]);
   }
}



// Propagate state via Runge Kutta Fehlberg 4/5 integration scheme.
IntegratorResult
RKFehlberg45FirstOrderODEIntegrator::integrate (
   double dt,
   unsigned int target_stage,
   const double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   // Integrate to the next stage
   switch (target_stage) {

   // Initial stage (stage 1):
   // Save initial state and update per RKF45 RKa[1] (one element).
   case 1:
      integ_utils::inplace_euler_step_save_both (
         velocity,
         RKFehlberg45ButcherTableau::RKa[1][0]*dt, state_size,
         init_state, deriv_hist[0], position);
      step_factor = RKFehlberg45ButcherTableau::RKc[1];
      break;

   // Intermediate stages (2 to 5):
   // Update state per RKF45 RKa[target_stage] (target_stage elements).
   case 2:
      integ_utils::weighted_step_save_deriv<2> (
         init_state, velocity,
         RKFehlberg45ButcherTableau::RKa[2],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKFehlberg45ButcherTableau::RKc[2];
      break;

   case 3:
      integ_utils::weighted_step_save_deriv<3> (
         init_state, velocity,
         RKFehlberg45ButcherTableau::RKa[3],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKFehlberg45ButcherTableau::RKc[3];
      break;

   case 4:
      integ_utils::weighted_step_save_deriv<4> (
         init_state, velocity,
         RKFehlberg45ButcherTableau::RKa[4],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKFehlberg45ButcherTableau::RKc[4];
      break;

   case 5:
      integ_utils::weighted_step_save_deriv<5> (
         init_state, velocity,
         RKFehlberg45ButcherTableau::RKa[5],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKFehlberg45ButcherTableau::RKc[5];
      break;

   // Final stage (6):
   // Update state per RKF45 RKb5 (6 elements).
   case 6:
      integ_utils::weighted_step<6> (
         init_state, velocity, deriv_hist,
         RKFehlberg45ButcherTableau::RKb5, dt, state_size,
         position);
      step_factor = RKFehlberg45ButcherTableau::RKc[5];
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

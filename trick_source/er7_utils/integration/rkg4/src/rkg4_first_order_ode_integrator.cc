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
 * Defines member functions for the class RKGill4FirstOrderODEIntegrator.
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
#include "../include/rkg4_butcher_tableau.hh"
#include "../include/rkg4_first_order_ode_integrator.hh"


namespace er7_utils {

// RKGill4FirstOrderODEIntegrator default constructor.
RKGill4FirstOrderODEIntegrator::RKGill4FirstOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (),
   init_state (NULL)
{
   alloc::initialize_2D_array<double, 4> (deriv_hist);
}


// RKGill4FirstOrderODEIntegrator copy constructor.
RKGill4FirstOrderODEIntegrator::RKGill4FirstOrderODEIntegrator (
   const RKGill4FirstOrderODEIntegrator & src)
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


// RKGill4FirstOrderODEIntegrator non-default constructor.
RKGill4FirstOrderODEIntegrator::RKGill4FirstOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (size, controls),
   init_state (NULL)
{

   // Allocate memory used by Runge Kutta Gill 4 algorithm.
   init_state = alloc::allocate_array<double> (state_size);
   alloc::allocate_2D_array<double, 4> (state_size, deriv_hist);
}


// RKGill4FirstOrderODEIntegrator destructor.
RKGill4FirstOrderODEIntegrator::~RKGill4FirstOrderODEIntegrator (
   void)
{
   alloc::deallocate_array<double> (init_state);
   alloc::deallocate_2D_array<double, 4> (deriv_hist);
}


// Clone a RKGill4FirstOrderODEIntegrator.
RKGill4FirstOrderODEIntegrator *
RKGill4FirstOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Non-throwing swap.
void
RKGill4FirstOrderODEIntegrator::swap (
   RKGill4FirstOrderODEIntegrator & other)
{
   FirstOrderODEIntegrator::swap (other);
   std::swap (init_state, other.init_state);
   std::swap (deriv_hist[0], other.deriv_hist[0]);
   std::swap (deriv_hist[1], other.deriv_hist[1]);
   std::swap (deriv_hist[2], other.deriv_hist[2]);
   std::swap (deriv_hist[3], other.deriv_hist[3]);
}



// Propagate state via fourth order Runge Kutta Gill.
IntegratorResult
RKGill4FirstOrderODEIntegrator::integrate (
   double dt,
   unsigned int target_stage,
   const double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   // Advance state.
   switch (target_stage) {
   case 1:
      integ_utils::inplace_euler_step_save_both (
         velocity,
         RKGill4ButcherTableau::RKa[1][0]*dt, state_size,
         init_state, deriv_hist[0], position);
      step_factor = 0.5;
      break;

   case 2:
      integ_utils::weighted_step_save_deriv<2> (
         init_state, velocity,
         RKGill4ButcherTableau::RKa[2],
         dt, state_size,
         deriv_hist, position);
      step_factor = 0.5;
      break;

   case 3:
      integ_utils::weighted_step_save_deriv<2> (
         init_state, velocity,
         RKGill4ButcherTableau::RKa[3]+1,
         dt, state_size,
         deriv_hist+1, position);
      step_factor = 1.0;
      break;

   case 4:
      integ_utils::weighted_step<4> (
         init_state, velocity, deriv_hist,
         RKGill4ButcherTableau::RKb,
         dt, state_size,
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

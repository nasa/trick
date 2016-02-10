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
 * Defines member functions for the class RKFehlberg78FirstOrderODEIntegrator.
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
#include "../include/rkf78_first_order_ode_integrator.hh"
#include "../include/rkf78_butcher_tableau.hh"


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

// RK4FirstOrderODEIntegrator default constructor.
RKFehlberg78FirstOrderODEIntegrator::RKFehlberg78FirstOrderODEIntegrator (
   void)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (),
   init_state (NULL)
{
   alloc::initialize_2D_array<13> (deriv_hist);
}


// RK4FirstOrderODEIntegrator copy constructor.
RKFehlberg78FirstOrderODEIntegrator::RKFehlberg78FirstOrderODEIntegrator (
   const RKFehlberg78FirstOrderODEIntegrator & src)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (src),
   init_state (NULL)
{
   if (src.init_state != NULL) {
      init_state = alloc::replicate_array (state_size, src.init_state);
      alloc::replicate_2D_array<13> (state_size, src.deriv_hist, deriv_hist);
   }
   else {
      alloc::initialize_2D_array<13> (deriv_hist);
   }
}


// RK4FirstOrderODEIntegrator non-default constructor.
RKFehlberg78FirstOrderODEIntegrator::RKFehlberg78FirstOrderODEIntegrator (
   unsigned int size,
   IntegrationControls & controls)
:
   Er7UtilsDeletable (),
   FirstOrderODEIntegrator (size, controls),
   init_state (NULL)
{

   // Allocate memory used by Runge Kutta Fehlberg 7/8 algorithm.
   init_state = alloc::allocate_array (state_size);
   alloc::allocate_2D_array<13> (state_size, deriv_hist);
}


// RKFehlberg78FirstOrderODEIntegrator destructor.
RKFehlberg78FirstOrderODEIntegrator::~RKFehlberg78FirstOrderODEIntegrator (
   void)
{
   alloc::deallocate_array (init_state);
   alloc::deallocate_2D_array<13> (deriv_hist);
}


// Clone a RKFehlberg78FirstOrderODEIntegrator.
RKFehlberg78FirstOrderODEIntegrator *
RKFehlberg78FirstOrderODEIntegrator::create_copy ()
const
{
   return alloc::replicate_object (*this);
}


// Non-throwing swap.
void
RKFehlberg78FirstOrderODEIntegrator::swap (
   RKFehlberg78FirstOrderODEIntegrator & other)
{
   FirstOrderODEIntegrator::swap (other);

   std::swap (init_state, other.init_state);
   for (int ii = 0; ii < 13; ++ii) {
      std::swap (deriv_hist[ii], other.deriv_hist[ii]);
   }
}



// Propagate state via Runge Kutta Fehlberg 7/8 integration scheme.
IntegratorResult
RKFehlberg78FirstOrderODEIntegrator::integrate (
   double dt,
   unsigned int target_stage,
   const double * ER7_UTILS_RESTRICT velocity,
   double * ER7_UTILS_RESTRICT position)
{
   double step_factor;

   // Integrate to the next stage
   switch (target_stage) {

   // Initial stage (stage 1):
   // Save initial state and update per RKF78 RKa[1] (one element).
   case 1:
      integ_utils::inplace_euler_step_save_both (
         velocity,
         RKF78_BUTCHER_TABLEAU::RKa[1][0]*dt, state_size,
         init_state, deriv_hist[0], position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[1];
      break;

   // Intermediate stages (2 to 12):
   // Update state per RKF78 RKa[target_stage] (target_stage elements).
   case 2:
      integ_utils::weighted_step_save_deriv<2> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[2],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[2];
      break;

   case 3:
      integ_utils::weighted_step_save_deriv<3> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[3],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[3];
      break;

   case 4:
      integ_utils::weighted_step_save_deriv<4> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[4],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[4];
      break;

   case 5:
      integ_utils::weighted_step_save_deriv<5> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[5],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[5];
      break;

   case 6:
      integ_utils::weighted_step_save_deriv<6> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[6],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[6];
      break;

   case 7:
      integ_utils::weighted_step_save_deriv<7> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[7],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[7];
      break;

   case 8:
      integ_utils::weighted_step_save_deriv<8> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[8],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[8];
      break;

   case 9:
      integ_utils::weighted_step_save_deriv<9> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[9],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[9];
      break;

   case 10:
      integ_utils::weighted_step_save_deriv<10> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[10],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[10];
      break;

   case 11:
      integ_utils::weighted_step_save_deriv<11> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[11],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[11];
      break;

   case 12:
      integ_utils::weighted_step_save_deriv<12> (
         init_state, velocity,
         RKF78_BUTCHER_TABLEAU::RKa[12],
         dt, state_size,
         deriv_hist, position);
      step_factor = RKF78_BUTCHER_TABLEAU::RKc[12];
      break;

   // Final stage (13):
   // Update state per RKF78 RKb8 (13 elements).
   case 13:
      integ_utils::weighted_step<13> (
         init_state, velocity, deriv_hist,
         RKF78_BUTCHER_TABLEAU::RKb8, dt, state_size,
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

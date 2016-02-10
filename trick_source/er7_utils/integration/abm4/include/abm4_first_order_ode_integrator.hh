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
 * Defines the class ABM4FirstOrderODEIntegrator, which integrates a monolithic
 * state as a first order ODE.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_ABM4_ONE_STATE_INTEGRATOR_HH
#define ER7_UTILS_ABM4_ONE_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/priming_first_order_ode_integrator.hh"


namespace er7_utils {

class IntegratorConstructor;


/**
 * Advance state using the fourth order Adams-Bashforth-Moulton method.
 * This includes
 *   - Initial priming of the recent history of state derivatives,
 *   - Using the ABM-4 method once primed, and
 *   - Clearing the history when needed (e.g., time change or state change).
 */
class ABM4FirstOrderODEIntegrator : public PrimingFirstOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(ABM4FirstOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * ABM4FirstOrderODEIntegrator default constructor.
    */
   ABM4FirstOrderODEIntegrator (void);

   /**
    * ABM4FirstOrderODEIntegrator copy constructor.
    * @param[in] src  Item to be copied.
    */
   ABM4FirstOrderODEIntegrator (
      const ABM4FirstOrderODEIntegrator & src);

   /**
    * ABM4FirstOrderODEIntegrator non-default constructor.
    * This is the constructor used by the integrator constructor.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     size                State size
    * @param[in,out] controls_in         Integration controls
    */
   ABM4FirstOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int size,
      IntegrationControls & controls_in);

   /**
    * ABM4FirstOrderODEIntegrator destructor.
    */
   virtual ~ABM4FirstOrderODEIntegrator (void);

   /**
    * ABM4FirstOrderODEIntegrator assignment operator.
    * @param src  Item to be copied.
    */
   ABM4FirstOrderODEIntegrator & operator=(
      ABM4FirstOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' ABM4FirstOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual ABM4FirstOrderODEIntegrator * create_copy () const;


protected:

   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (ABM4FirstOrderODEIntegrator & other);

   using PrimingFirstOrderODEIntegrator::swap;

   /**
    * Save derivatives during priming.
    * @param[in] countdown     The prime_counter data member.
    * @param[in] position      Generalized position vector.
    * @param[in] velocity      Generalized velocity vector.
    */
   virtual void technique_save_derivatives (
      int countdown,
      const double * ER7_UTILS_RESTRICT velocity,
      const double * ER7_UTILS_RESTRICT position);

   /**
    * Propagate state via fourth order Adams Bashforth Moulton.
    * @param[in]     dyn_dt        Integration step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in]     velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult technique_integrate (
      double dyn_dt,
      unsigned int target_stage,
      const double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position);


   // Member data

   double * init_state; /*!< trick_units(--) @n
      The state at the start of an integration cycle. */

   double * deriv_hist[4]; /*!< trick_units(--) @n
      The state derivatives at the start of current and at the starts of the
      three previous integration cycles.

      This array is rotated so that deriv_hist[0] contains the state derivatives
      at the start of the current cycle, deriv_hist[1] the derivatives at the
      start of the previous cycle, and so on. */
};

}


#ifdef ER7_UTILS_NEED_AUX_INCLUDES
#include "er7_utils/integration/core/include/integration_controls.hh"
#endif


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

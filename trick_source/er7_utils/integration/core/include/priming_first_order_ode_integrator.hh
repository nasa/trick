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
 * Defines the class PrimingFirstOrderODEIntegrator, which serves as the basis
 * for those first order ODE integrators that need priming.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_PRIMING_ONE_STATE_INTEGRATOR_HH
#define ER7_UTILS_PRIMING_ONE_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/first_order_ode_integrator.hh"


namespace er7_utils {

class IntegratorConstructor;


/**
 * Advance state for a technique that needs priming.
 * This includes
 *   - Initial priming of the recent history of state derivatives,
 *   - Using the technique-specific integration method once primed, and
 *   - Clearing the history when needed (e.g., time change or state change).
 */
class PrimingFirstOrderODEIntegrator : public FirstOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(PrimingFirstOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * PrimingFirstOrderODEIntegrator destructor.
    */
   virtual ~PrimingFirstOrderODEIntegrator (void);


   // Member functions.

   /**
    * Set the controls object that guides this object's integration process.
    * @param[in,out] controls_in  Integration controls
    */
   virtual void set_controls (IntegrationControls & controls_in);

   /**
    * Reset a PrimingFirstOrderODEIntegrator.
    */
   virtual void reset_integrator (void)
   {
      prime_counter = priming_state_size;
      primed = false;
      primer->reset_integrator();
   }

   /**
    * Propagate state. This integrator covers priming, but defers
    * technique-specific integration to the technique.
    * @param[in]     dyn_dt        Integration step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in]     velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult integrate (
      double dyn_dt,
      unsigned int target_stage,
      const double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position);


protected:

   // Constructors.

   /**
    * PrimingFirstOrderODEIntegrator default constructor.
    */
   PrimingFirstOrderODEIntegrator (void);

   /**
    * PrimingFirstOrderODEIntegrator copy constructor.
    */
   PrimingFirstOrderODEIntegrator (
      const PrimingFirstOrderODEIntegrator &);

   /**
    * PrimingFirstOrderODEIntegrator non-default constructor.
    * This is the constructor used by the integrator constructor.
    * @param[in]     priming_size        Number of required derivatives
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     size                State size
    * @param[in,out] controls_in         Integration controls
    */
   PrimingFirstOrderODEIntegrator (
      unsigned int priming_size,
      const IntegratorConstructor & primer_constructor,
      unsigned int size,
      IntegrationControls & controls_in);


   // Member functions

   /**
    * Swap contents with that of another.
    * @param other  Other object with which contents are to be swapped.
    */
   virtual void swap (PrimingFirstOrderODEIntegrator & other);

   using FirstOrderODEIntegrator::swap;

   /**
    * Save derivatives during priming.
    * @param[in] countdown     The prime_counter data member.
    * @param[in] velocity      Generalized velocity vector.
    * @param[in] position      Generalized position vector.
    */
   virtual void technique_save_derivatives (
      int countdown,
      const double * ER7_UTILS_RESTRICT velocity,
      const double * ER7_UTILS_RESTRICT position) = 0;

   /**
    * Propagate state once primed, using the technique-specific algorithm.
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
      double * ER7_UTILS_RESTRICT position) = 0;


   // Member data

   IntegrationControls * controls; /**< trick_units(--) @n
      The integration controls object that controls this state integrator. */

   FirstOrderODEIntegrator * primer; /**< trick_units(--) @n
      The state integrator used to prime this integrator. */

   int priming_state_size; /**< trick_units(--) @n
      The number of start of cycle derivatives needed by the technique. */

   int prime_counter; /*!< trick_units(--) @n
      Priming phase countdown. This is set to priming_state_size initially
      and upon reset. Priming is complete when the counter reaches zero. */

   bool primed; /*!< trick_units(--) @n
      Indicates whether the object has been primed.
      This is false initially and upon reset; it is set to true when
      priming is complete. */


private:

   /**
    * Not implemented.
    */
   PrimingFirstOrderODEIntegrator & operator= (
      const PrimingFirstOrderODEIntegrator &);
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

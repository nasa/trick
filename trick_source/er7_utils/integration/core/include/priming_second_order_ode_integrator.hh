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
 * Defines the class PrimingSecondOrderODEIntegrator, which serves as the basis
 * for those second order ODE integrators that need priming.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_PRIMING_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_PRIMING_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

class IntegratorConstructor;


/**
 * Advance state for a technique that needs priming.
 * This includes
 *   - Initial priming of the recent history of state derivatives,
 *   - Using the technique-specific integration method once primed, and
 *   - Clearing the history when needed (e.g., time change or state change).
 */
class PrimingSecondOrderODEIntegrator : public SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(PrimingSecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * PrimingSecondOrderODEIntegrator destructor.
    */
   virtual ~PrimingSecondOrderODEIntegrator (void);


   // Member functions.

   /**
    * Set the controls object that guides this object's integration process.
    * The default implementation does nothing.
    * @param[in,out] controls_in  Integration controls
    */
   virtual void set_controls (IntegrationControls & controls_in);

   /**
    * Reset a PrimingSecondOrderODEIntegrator.
    */
   virtual void reset_integrator (void)
   {
      // Mark this object as unprimed.
      prime_counter = priming_state_size;
      primed = false;

      // Reset the primer.
      primer->reset_integrator();
   }


   /**
    * Reset the position derivative functions.
    * @param[in] deriv_funs  Position time derivative functions container
    */
   virtual void set_position_derivative_functions (
      const GeneralizedPositionDerivativeFunctions & deriv_funs)
   {
      // Set our derivative functions.
      SecondOrderODEIntegrator::set_position_derivative_functions (deriv_funs);

      // And also that of the primer.
      primer->set_position_derivative_functions (deriv_funs);
   }


   /**
    * Reset the position advance functions.
    * @param[in] step_funs Position step functions container
    */
   virtual void set_position_step_functions (
      const GeneralizedPositionStepFunctions & step_funs)
   {
      // Set our step functions.
      SecondOrderODEIntegrator::set_position_step_functions (step_funs);

      // And also that of the primer.
      primer->set_position_step_functions (step_funs);
   }


   /**
    * Propagate state. This integrator covers priming, but defers
    * technique-specific integration to the technique.
    * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in]     accel         Time derivative of the generalized velocity.
    * @param[in,out] velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult integrate (
      double dyn_dt,
      unsigned int target_stage,
      double const * ER7_UTILS_RESTRICT accel,
      double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position);


protected:

   // Constructors.

   /**
    * PrimingSecondOrderODEIntegrator default constructor.
    */
   PrimingSecondOrderODEIntegrator (void);

   /**
    * PrimingSecondOrderODEIntegrator copy constructor.
    */
   PrimingSecondOrderODEIntegrator (
      const PrimingSecondOrderODEIntegrator &);

   /**
    * PrimingSecondOrderODEIntegrator non-default constructor for a simple
    * simple second order ODE.
    * This is the constructor invoked by the constructors of simple second
    * order ODE integrator classes that derive from this class.
    * @param[in]     priming_size        Number of required derivatives
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     size                State size
    * @param[in,out] controls_in         Integration controls
    */
   PrimingSecondOrderODEIntegrator (
      unsigned int priming_size,
      const IntegratorConstructor & primer_constructor,
      unsigned int size,
      IntegrationControls & controls_in);

   /**
    * PrimingSecondOrderODEIntegrator non-default constructor for a generalized
    * second order ODE in which the time derivative of position is computed
    * via the provided derivative function. Position is integrated directly
    * using the computed derivative.
    * This is the constructor invoked by the constructors of generalized
    * second order ODE integrator classes that derive from this class.
    * @param[in]     priming_size        Number of required derivatives
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     deriv_funs          Position derivative functions container
    * @param[in,out] controls_in         Integration controls
    */
   PrimingSecondOrderODEIntegrator (
      unsigned int priming_size,
      const IntegratorConstructor & primer_constructor,
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls_in);

   /**
    * PrimingSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided position step function.
    * This is the constructor invoked by the constructors of generalized
    * second order ODE integrator classes that derive from this class.
    * @param[in]     priming_size        Number of required derivatives
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     step_funs           Position step functions container
    * @param[in,out] controls_in         Integration controls
    */
   PrimingSecondOrderODEIntegrator (
      unsigned int priming_size,
      const IntegratorConstructor & primer_constructor,
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls_in);


   // Member functions

   /**
    * Swap contents with that of another.
    * @param other  Other object with which contents are to be swapped.
    */
   virtual void swap (PrimingSecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;

   /**
    * Save derivatives during priming.
    * @param[in] countdown     The prime_counter data member.
    * @param[in] accel         Time derivative of the generalized velocity.
    * @param[in] velocity      Generalized velocity vector.
    * @param[in] position      Generalized position vector.
    */
   virtual void technique_save_derivatives (
      int countdown,
      const double * ER7_UTILS_RESTRICT accel,
      const double * ER7_UTILS_RESTRICT velocity,
      const double * ER7_UTILS_RESTRICT position) = 0;

   /**
    * Propagate state once primed, using the technique-specific algorithm.
    * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in]     accel         Time derivative of the generalized velocity.
    * @param[in,out] velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult technique_integrate (
      double dyn_dt,
      unsigned int target_stage,
      double const * ER7_UTILS_RESTRICT accel,
      double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position) = 0;


   // Member data

   IntegrationControls * controls; /**< trick_units(--) @n
      The integration controls object that controls this state integrator. */

   SecondOrderODEIntegrator * primer; /**< trick_units(--) @n
      The state integrator used to prime this integrator. */

   int priming_state_size; /**< trick_units(--) @n
      The number of start of cycle derivatives needed by the technique. */

   int prime_counter; /**< trick_units(--) @n
      Priming phase countdown. This is set to priming_state_size initially
      and upon reset. Priming is complete when the counter reaches zero. */

   bool primed; /**< trick_units(--) @n
      Indicates whether the object has been primed.
      This is false initially and upon reset; it is set to true when
      priming is complete. */


private:

   /**
    * Not implemented.
    */
   PrimingSecondOrderODEIntegrator & operator= (
      const PrimingSecondOrderODEIntegrator &);
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

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
 * Defines the class SecondOrderODEIntegrator, the base class for propagating
 * state that conceptually comprises a zeroth and first derivative.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_TWO_STATE_INTEGRATOR_HH

// System includes
#include <cstddef>

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Local includes
#include "state_integrator.hh"
#include "generalized_position_derivative.hh"
#include "integration_technique.hh"
#include "integrator_result.hh"


namespace er7_utils {

class IntegrationControls;

/**
 * Base class for propagating states comprising a zeroth and first derivative.
 */
class SecondOrderODEIntegrator : public StateIntegratorInterface {

ER7_UTILS_MAKE_SIM_INTERFACES(SecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * SecondOrderODEIntegrator destructor.
    */
   virtual ~SecondOrderODEIntegrator (void)
   { }


   // Member functions.

   /**
    * Create a copy of 'this', a derived SecondOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual SecondOrderODEIntegrator* create_copy () const = 0;

   /**
    * Set the controls object that guides this object's integration process.
    * The default implementation does nothing.
    * @param[in,out] controls  Integration controls (unused)
    */
   virtual void set_controls (IntegrationControls & controls ER7_UTILS_UNUSED)
   { }

   /**
    * Reset the position derivative functions.
    * This function exists so that the derivative function pointers can be
    * restored to their checkpointed value at restart time.
    * @param[in] deriv_funs  Position derivative functions container
    */
   virtual void set_position_derivative_functions (
      const GeneralizedPositionDerivativeFunctions & deriv_funs);

   /**
    * Reset the position step functions.
    * This function exists so that the step function pointers can be
    * restored to their checkpointed value at restart time.
    * @param[in] step_funs  Lie integrator step functions container
    */
   virtual void set_position_step_functions (
      const GeneralizedPositionStepFunctions & step_funs);

   /**
    * Propagate state to the specified stage of the integration
    * process for an overall integration time interval of dyn_dt.
    *
    * All integration techniques can assume that the status parameter will
    * be default-initialized on entry: time scales set to 1.0, failure status
    * and merge count set to zero. An integration technique only needs to change
    * the time scales if they are something other than 1.0, the failure mode
    * if the technique somehow is not successful.
    *
    * Note that this is a pure virtual function; instantiable subclasses must
    * provide an implementation of this method.
    *
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
      double * ER7_UTILS_RESTRICT position) = 0;


protected:

   // Constructors.

   /**
    * SecondOrderODEIntegrator default constructor.
    * This is the constructor invoked by the default constructors of classes
    * that derive from this class.
    * Note that this default constructor does not create a usable instance.
    */
   SecondOrderODEIntegrator (void);

   /**
    * SecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied
    */
   SecondOrderODEIntegrator (const SecondOrderODEIntegrator & src);

   /**
    * SecondOrderODEIntegrator non-default constructor for a simple second
    * order ODE, one in the velocity is the time derivative of position.
    * This is the constructor invoked by the constructors of simple second
    * order ODE integrator classes that derive from this class.
    * @param[in]     size      State size
    * @param[in,out] controls  Integration controls (unused)
    */
   SecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * SecondOrderODEIntegrator non-default constructor for a generalized
    * second order ODE in which the time derivative of position is computed
    * via the provided derivative function. Position is integrated directly
    * using the computed derivative.
    * This is the constructor invoked by the constructors of generalized
    * second order ODE integrator classes that derive from this class.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Derivative functions container
    * @param[in,out] controls       Integration controls (unused)
    */
   SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

   /**
    * SecondOrderODEIntegrator non-default constructor for a generalized
    * second order ODE in which the time derivative of position is computed
    * via the provided derivative function. Position is integrated indirectly
    * using the provided step function.
    * This is the constructor invoked by the constructors of generalized
    * second order ODE integrator classes that derive from this class.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Step functions container
    * @param[in,out] controls       Integration controls (unused)
    */
   SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Swap contents with that of other.
    * @param other  Other object with which contents are to be swapped.
    */
   virtual void swap (SecondOrderODEIntegrator & other);


   // Member data.

   GeneralizedPositionDerivativeFunctions::FirstDerivative
   compute_posdot; /**< trick_io(**) @n
      External function that computes the time derivative of
      generalized position. */

   GeneralizedPositionDerivativeFunctions::SecondDerivative
   compute_posdotdot; /**< trick_io(**) @n
      External function that computes the second time derivative of
      generalized position. */

   GeneralizedPositionStepFunctions::ExpMapPositionStep
   compute_expmap_position_step; /**< trick_io(**) @n
      External function that advances generalized position. */

   GeneralizedPositionStepFunctions::DexpinvTransformVelocity
   compute_dexpinv_velocity_transform; /**< trick_io(**) @n
      External function that transforms velocity per the Lie algebra
      dexpinv operator. */

   int state_size[2]; /**< trick_units(--) @n
      The sizes of the generalized position and generalized velocity vectors. */

   Integration::ODEProblemType problem_type; /**< trick_units(--) @n
      The type of problem to be solved. */


private:

   /**
    * Not implemented.
    */
   SecondOrderODEIntegrator & operator= (const SecondOrderODEIntegrator &);
};


}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

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
 * Defines the class RK4SecondOrderODEIntegrator, which integrates a state
 * comprising a zeroth derivative / first derivative pair via the RK4 method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RK4_TWO_STATE_INTEGRATOR_BASE_HH
#define ER7_UTILS_RK4_TWO_STATE_INTEGRATOR_BASE_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Base class for integrating a second order ODE using the standard
 * 4th order Runge-Kutta method.
 * This class also serves as the base for the second order ODE integrators
 * that use 4th order Runga-Kutta-Gill.
 */
class RK4SecondOrderODEIntegrator : public SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RK4SecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * RK4SecondOrderODEIntegrator destructor.
    */
   virtual ~RK4SecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * RK4SecondOrderODEIntegrator default constructor.
    * Notes:
    *  - This implicitly assumes the time derivative of position is velocity.
    *  - This is needed so that Trick can checkpoint/restart its integrators.
    */
   RK4SecondOrderODEIntegrator (void);

   /**
    * RK4SecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK4SecondOrderODEIntegrator (const RK4SecondOrderODEIntegrator & src);

   /**
    * RK4SecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE, one in which generalized velocity
    * is time derivative of generalized position.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   RK4SecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * RK4SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position derivative function.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RK4SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

   /**
    * RK4SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position step function.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RK4SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (RK4SecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * init_pos; /**< trick_units(--) @n
      Position at the start of an integration cycle. */

   double * init_vel; /**< trick_units(--) @n
      Velocity at the start of an integration cycle. */

   double * saved_pos; /**< trick_units(--) @n
      Position after stage 1. */

   double * dtheta; /**< trick_units(--) @n
      Product of delta t and weighted sum of generalized velocities. */

   double * posdot_hist[4]; /**< trick_units(--) @n
      Position derivatives at each step in the integration cycle. */

   double * veldot_hist[4]; /**< trick_units(--) @n
      Velocity derivatives at each step in the integration cycle. */


private:

   /**
    * Not implemented.
    */
   RK4SecondOrderODEIntegrator & operator= (
      const RK4SecondOrderODEIntegrator &);
};

}

#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

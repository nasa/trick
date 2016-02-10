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
 * Defines the class RK2MidpointSecondOrderODEIntegrator, which integrates a
 * state comprising a zeroth derivative / first derivative pair via the
 * RK2 midpoint method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RK2_MIDPOINT_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_RK2_MIDPOINT_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using 2nd order Runge-Kutta (midpoint method).
 */
class RK2MidpointSecondOrderODEIntegrator : public SecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(RK2MidpointSecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * RK2MidpointSecondOrderODEIntegrator destructor.
    */
   virtual ~RK2MidpointSecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * RK2MidpointSecondOrderODEIntegrator default constructor.
    * This is needed so that Trick can checkpoint/restart its integrators.
    */
   RK2MidpointSecondOrderODEIntegrator (void);

   /**
    * RK2MidpointSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK2MidpointSecondOrderODEIntegrator (
      const RK2MidpointSecondOrderODEIntegrator & src);

   /**
    * RK2MidpointSecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE.
    * This constructor is used by the integrator constructor.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   RK2MidpointSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * RK2MidpointSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided derivative function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RK2MidpointSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

   /**
    * RK2MidpointSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided position step function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RK2MidpointSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (RK2MidpointSecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * init_pos; /**< trick_units(--) @n
      Position at the start of an integration cycle. */

   double * init_vel; /**< trick_units(--) @n
      Velocity at the start of an integration cycle. */

   double * posdot; /**< trick_units(--) @n
      Position derivatives. */

   double * dtheta; /**< trick_units(--) @n
      Position step. */


private:

   /**
    * Not implemented.
    */
   RK2MidpointSecondOrderODEIntegrator & operator= (
      const RK2MidpointSecondOrderODEIntegrator &);
};


/**
 * Specialization of RK2MidpointSecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class RK2MidpointSimpleSecondOrderODEIntegrator :
   public RK2MidpointSecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(RK2MidpointSimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK2MidpointSimpleSecondOrderODEIntegrator default constructor.
    */
   RK2MidpointSimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      RK2MidpointSecondOrderODEIntegrator()
   {}

   /**
    * RK2MidpointSimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK2MidpointSimpleSecondOrderODEIntegrator (
      const RK2MidpointSimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      RK2MidpointSecondOrderODEIntegrator(src)
   {}

   /**
    * RK2MidpointSimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   RK2MidpointSimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      RK2MidpointSecondOrderODEIntegrator(size, controls)
   {}

   /**
    * RK2MidpointSimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~RK2MidpointSimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RK2MidpointSimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RK2MidpointSimpleSecondOrderODEIntegrator & operator= (
      RK2MidpointSimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' RK2MidpointSimpleSecondOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual RK2MidpointSimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state using RK2 midpoint.
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
};


/**
 * Specialization of RK2MidpointSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class RK2MidpointGeneralizedDerivSecondOrderODEIntegrator :
   public RK2MidpointSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   RK2MidpointGeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK2MidpointGeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   RK2MidpointGeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      RK2MidpointSecondOrderODEIntegrator()
   {}

   /**
    * RK2MidpointGeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK2MidpointGeneralizedDerivSecondOrderODEIntegrator (
      const RK2MidpointGeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      RK2MidpointSecondOrderODEIntegrator(src)
   {}

   /**
    * RK2MidpointGeneralizedDerivSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RK2MidpointGeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RK2MidpointSecondOrderODEIntegrator (position_size, velocity_size,
                                           deriv_funs, controls)
   { }

   /**
    * RK2MidpointGeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~RK2MidpointGeneralizedDerivSecondOrderODEIntegrator (void)
   { }


   // Member functions.

   /**
    * RK2MidpointGeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RK2MidpointGeneralizedDerivSecondOrderODEIntegrator & operator= (
      RK2MidpointGeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RK2MidpointGeneralizedDerivSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state using RK2 midpoint.
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
};


/**
 * Specialization of RK2MidpointSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class RK2MidpointGeneralizedStepSecondOrderODEIntegrator :
   public RK2MidpointSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   RK2MidpointGeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK2MidpointGeneralizedStepSecondOrderODEIntegrator default constructor.
    */
   RK2MidpointGeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      RK2MidpointSecondOrderODEIntegrator()
   {}

   /**
    * RK2MidpointGeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK2MidpointGeneralizedStepSecondOrderODEIntegrator (
      const RK2MidpointGeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      RK2MidpointSecondOrderODEIntegrator(src)
   {}

   /**
    * RK2MidpointGeneralizedStepSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RK2MidpointGeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RK2MidpointSecondOrderODEIntegrator (position_size, velocity_size,
                                           step_funs, controls)
   { }

   /**
    * RK2MidpointGeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~RK2MidpointGeneralizedStepSecondOrderODEIntegrator (void)
   { }


   // Member functions.

   /**
    * RK2MidpointGeneralizedStepSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RK2MidpointGeneralizedStepSecondOrderODEIntegrator & operator= (
      RK2MidpointGeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RK2MidpointGeneralizedStepSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state using RK2 midpoint.
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
};

}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

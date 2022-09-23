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
 * Defines the class VelocityVerletSecondOrderODEIntegrator, which integrates a
 * state comprising a zeroth derivative / first derivative pair via
 * velocity verlet.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_VELOCITY_VERLET_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_VELOCITY_VERLET_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using the velocity verlet method.
 */
class VelocityVerletSecondOrderODEIntegrator : public SecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(VelocityVerletSecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * VelocityVerletSecondOrderODEIntegrator destructor.
    */
   virtual ~VelocityVerletSecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * VelocityVerletSecondOrderODEIntegrator default constructor.
    */
   VelocityVerletSecondOrderODEIntegrator (void);

   /**
    * VelocityVerletSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   VelocityVerletSecondOrderODEIntegrator (
      const VelocityVerletSecondOrderODEIntegrator & src);

   /**
    * VelocityVerletSecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE, one in which generalized velocity
    * is time derivative of generalized position.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   VelocityVerletSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * VelocityVerletSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided derivative function.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   VelocityVerletSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

   /**
    * VelocityVerletSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided position step function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   VelocityVerletSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (VelocityVerletSecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * init_vel; /**< trick_units(--) @n
      Velocity at the start of the integration interval. */

   double * init_acc; /**< trick_units(--) @n
      Acceleration at the start of the integration interval. */

   double * dtheta; /**< trick_units(--) @n
      Generalized position step. */

   double * posdot; /**< trick_units(--) @n
      Position derivative at the first step of the integration cycle. */


private:

   /**
    * Not implemented.
    */
   VelocityVerletSecondOrderODEIntegrator & operator= (
      const VelocityVerletSecondOrderODEIntegrator &);
};


/**
 * Specialization of VelocityVerletSecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class VelocityVerletSimpleSecondOrderODEIntegrator :
   public VelocityVerletSecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(VelocityVerletSimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * VelocityVerletSimpleSecondOrderODEIntegrator default constructor.
    */
   VelocityVerletSimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      VelocityVerletSecondOrderODEIntegrator()
   {}

   /**
    * VelocityVerletSimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   VelocityVerletSimpleSecondOrderODEIntegrator (
      const VelocityVerletSimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      VelocityVerletSecondOrderODEIntegrator(src)
   {}

   /**
    * VelocityVerletSimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   VelocityVerletSimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      VelocityVerletSecondOrderODEIntegrator(size, controls)
   {}

   /**
    * VelocityVerletSimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~VelocityVerletSimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * VelocityVerletSimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   VelocityVerletSimpleSecondOrderODEIntegrator & operator= (
      VelocityVerletSimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual VelocityVerletSimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state using velocity verlet.
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
 * Specialization of VelocityVerletSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class VelocityVerletGeneralizedDerivSecondOrderODEIntegrator :
   public VelocityVerletSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   VelocityVerletGeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * VelocityVerletGeneralizedDerivSecondOrderODEIntegrator
    * default constructor.
    */
   VelocityVerletGeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      VelocityVerletSecondOrderODEIntegrator()
   {}

   /**
    * VelocityVerletGeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   VelocityVerletGeneralizedDerivSecondOrderODEIntegrator (
      const VelocityVerletGeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      VelocityVerletSecondOrderODEIntegrator(src)
   {}

   /**
    * VelocityVerletGeneralizedDerivSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   VelocityVerletGeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      VelocityVerletSecondOrderODEIntegrator (
         position_size, velocity_size, deriv_funs, controls)
   {}

   /**
    * VelocityVerletGeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~VelocityVerletGeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

  /**
    * VelocityVerletGeneralizedDerivSecondOrderODEIntegrator
    * assignment operator.
    * @param[in] src  Object to be copied.
    */
   VelocityVerletGeneralizedDerivSecondOrderODEIntegrator & operator= (
      VelocityVerletGeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual VelocityVerletGeneralizedDerivSecondOrderODEIntegrator *
   create_copy () const;

   /**
    * Propagate state using velocity verlet.
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
 * Specialization of VelocityVerletSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class VelocityVerletGeneralizedStepSecondOrderODEIntegrator :
   public VelocityVerletSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   VelocityVerletGeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * VelocityVerletGeneralizedStepSecondOrderODEIntegrator default constructor.
    */
   VelocityVerletGeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      VelocityVerletSecondOrderODEIntegrator()
   {}

   /**
    * VelocityVerletGeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   VelocityVerletGeneralizedStepSecondOrderODEIntegrator (
      const VelocityVerletGeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      VelocityVerletSecondOrderODEIntegrator(src)
   {}

   /**
    * VelocityVerletGeneralizedStepSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   VelocityVerletGeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      VelocityVerletSecondOrderODEIntegrator (
         position_size, velocity_size, step_funs, controls)
   {}

   /**
    * VelocityVerletGeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~VelocityVerletGeneralizedStepSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * VelocityVerletGeneralizedStepSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   VelocityVerletGeneralizedStepSecondOrderODEIntegrator & operator= (
      VelocityVerletGeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual VelocityVerletGeneralizedStepSecondOrderODEIntegrator *
   create_copy () const;

   /**
    * Propagate state using velocity verlet.
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

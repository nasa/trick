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
 * Defines the class RK2HeunSecondOrderODEIntegrator, which integrates a state
 * comprising a zeroth derivative / first derivative pair via Heun's method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RK2_HEUN_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_RK2_HEUN_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using 2nd order Runge-Kutta (Heun's method).
 */
class RK2HeunSecondOrderODEIntegrator : public SecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(RK2HeunSecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * RK2HeunSecondOrderODEIntegrator destructor.
    */
   virtual ~RK2HeunSecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * RK2HeunSecondOrderODEIntegrator default constructor.
    * This is needed so that Trick can checkpoint/restart its integrators.
    */
   RK2HeunSecondOrderODEIntegrator (void);

   /**
    * RK2HeunSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK2HeunSecondOrderODEIntegrator (
      const RK2HeunSecondOrderODEIntegrator & src);

   /**
    * RK2HeunSecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE.
    * This constructor is used by the integrator constructor.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   RK2HeunSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * RK2HeunSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided derivative function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RK2HeunSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

   /**
    * RK2HeunSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided position step function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RK2HeunSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (RK2HeunSecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * init_pos; /**< trick_units(--) @n
      Position at the start of an integration cycle. */

   double * init_vel; /**< trick_units(--) @n
      Velocity at the start of an integration cycle. */

   double * init_acc; /**< trick_units(--) @n
      Velocity time derivative at the start of an integration cycle. */

   double * dtheta; /**< trick_units(--) @n
      Position step. */

   double * init_posdot; /**< trick_units(--) @n
      Position derivative at the start of an integration cycle. */

   double * final_posdot; /**< trick_units(--) @n
      Position derivative at the end of an integration cycle. */


private:

   /**
    * Not implemented.
    */
   RK2HeunSecondOrderODEIntegrator & operator= (
      const RK2HeunSecondOrderODEIntegrator &);
};


/**
 * Specialization of RK2HeunSecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class RK2HeunSimpleSecondOrderODEIntegrator :
   public RK2HeunSecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(RK2HeunSimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK2HeunSimpleSecondOrderODEIntegrator default constructor.
    */
   RK2HeunSimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      RK2HeunSecondOrderODEIntegrator()
   {}

   /**
    * RK2HeunSimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK2HeunSimpleSecondOrderODEIntegrator (
      const RK2HeunSimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      RK2HeunSecondOrderODEIntegrator(src)
   {}

   /**
    * RK2HeunSimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   RK2HeunSimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      RK2HeunSecondOrderODEIntegrator(size, controls)
   {}

   /**
    * RK2HeunSimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~RK2HeunSimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RK2HeunSimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RK2HeunSimpleSecondOrderODEIntegrator & operator= (
      RK2HeunSimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' RK2HeunSimpleSecondOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual RK2HeunSimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state using Heun's method.
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
 * Specialization of RK2HeunSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class RK2HeunGeneralizedDerivSecondOrderODEIntegrator :
   public RK2HeunSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RK2HeunGeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK2HeunGeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   RK2HeunGeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      RK2HeunSecondOrderODEIntegrator()
   {}

   /**
    * RK2HeunGeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK2HeunGeneralizedDerivSecondOrderODEIntegrator (
      const RK2HeunGeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      RK2HeunSecondOrderODEIntegrator(src)
   {}

   /**
    * RK2HeunGeneralizedDerivSecondOrderODEIntegrator non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RK2HeunGeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      RK2HeunSecondOrderODEIntegrator (position_size, velocity_size,
                                       deriv_funs, controls)
   {}

   /**
    * RK2HeunGeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~RK2HeunGeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RK2HeunGeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RK2HeunGeneralizedDerivSecondOrderODEIntegrator & operator= (
      RK2HeunGeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RK2HeunGeneralizedDerivSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state using Heun's method.
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
 * Specialization of RK2HeunSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class RK2HeunGeneralizedStepSecondOrderODEIntegrator :
   public RK2HeunSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RK2HeunGeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK2HeunGeneralizedStepSecondOrderODEIntegrator default constructor.
    */
   RK2HeunGeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      RK2HeunSecondOrderODEIntegrator()
   {}

   /**
    * RK2HeunGeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK2HeunGeneralizedStepSecondOrderODEIntegrator (
      const RK2HeunGeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      RK2HeunSecondOrderODEIntegrator(src)
   {}

   /**
    * RK2HeunGeneralizedStepSecondOrderODEIntegrator non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RK2HeunGeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      RK2HeunSecondOrderODEIntegrator (position_size, velocity_size,
                                       step_funs, controls)
   {}

   /**
    * RK2HeunGeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~RK2HeunGeneralizedStepSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RK2HeunGeneralizedStepSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RK2HeunGeneralizedStepSecondOrderODEIntegrator & operator= (
      RK2HeunGeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RK2HeunGeneralizedStepSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state using Heun's method.
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

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
 * Defines the class EulerSecondOrderODEIntegrator, which integrates a state
 * comprising a zeroth derivative / first derivative pair via Euler's method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_EULER_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_EULER_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using Euler's method.
 */
class EulerSecondOrderODEIntegrator : public SecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(EulerSecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * EulerSecondOrderODEIntegrator destructor.
    */
   virtual ~EulerSecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * EulerSecondOrderODEIntegrator default constructor.
    */
   EulerSecondOrderODEIntegrator (void);

   /**
    * EulerSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   EulerSecondOrderODEIntegrator (const EulerSecondOrderODEIntegrator & src);

   /**
    * EulerSecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE.
    * This constructor is used by the integrator constructor.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   EulerSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);


   /**
    * EulerSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided derivative function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   EulerSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

   /**
    * EulerSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided position step function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   EulerSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


protected:

   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (EulerSecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * posdot; /**< trick_units(--) @n
      Position derivative. */


private:

   /**
    * Not implemented.
    */
   EulerSecondOrderODEIntegrator & operator= (
      const EulerSecondOrderODEIntegrator &);
};


/**
 * Specialization of EulerSecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class EulerSimpleSecondOrderODEIntegrator :
   public EulerSecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(EulerSimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * EulerSimpleSecondOrderODEIntegrator default constructor.
    */
   EulerSimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      EulerSecondOrderODEIntegrator()
   {}

   /**
    * EulerSimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   EulerSimpleSecondOrderODEIntegrator (
      const EulerSimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      EulerSecondOrderODEIntegrator(src)
   {}

   /**
    * EulerSimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   EulerSimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      EulerSecondOrderODEIntegrator(size, controls)
   {}

   /**
    * EulerSimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~EulerSimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * EulerSimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   EulerSimpleSecondOrderODEIntegrator & operator= (
      EulerSimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' EulerSimpleSecondOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual EulerSimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state using Euler's method.
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
 * Specialization of EulerSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class EulerGeneralizedDerivSecondOrderODEIntegrator :
   public EulerSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(EulerGeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * EulerGeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   EulerGeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      EulerSecondOrderODEIntegrator()
   {}

   /**
    * EulerGeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   EulerGeneralizedDerivSecondOrderODEIntegrator (
      const EulerGeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      EulerSecondOrderODEIntegrator(src)
   {}

   /**
    * EulerGeneralizedDerivSecondOrderODEIntegrator non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Derivative functions container
    * @param[in,out] controls       Integration controls
    */
   EulerGeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      EulerSecondOrderODEIntegrator (position_size, velocity_size,
                                     deriv_funs, controls)
   {}

   /**
    * EulerGeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~EulerGeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * EulerGeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   EulerGeneralizedDerivSecondOrderODEIntegrator & operator= (
      EulerGeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual EulerGeneralizedDerivSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state using Euler's method.
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
 * Specialization of EulerSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class EulerGeneralizedStepSecondOrderODEIntegrator :
   public EulerSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(EulerGeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * EulerGeneralizedStepSecondOrderODEIntegrator default constructor.
    */
   EulerGeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      EulerSecondOrderODEIntegrator()
   {}

   /**
    * EulerGeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   EulerGeneralizedStepSecondOrderODEIntegrator (
      const EulerGeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      EulerSecondOrderODEIntegrator(src)
   {}

   /**
    * EulerGeneralizedStepSecondOrderODEIntegrator non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Step functions container
    * @param[in,out] controls       Integration controls
    */
   EulerGeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      EulerSecondOrderODEIntegrator (position_size, velocity_size,
                                     step_funs, controls)
   {}

   /**
    * EulerGeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~EulerGeneralizedStepSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * EulerGeneralizedStepSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   EulerGeneralizedStepSecondOrderODEIntegrator & operator= (
      EulerGeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual EulerGeneralizedStepSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state using Euler's method.
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

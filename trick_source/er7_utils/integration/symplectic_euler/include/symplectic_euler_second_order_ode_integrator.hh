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
 * Defines the class SymplecticEulerSecondOrderODEIntegrator, which integrates a
 * state comprising a zeroth derivative / first derivative pair via the
 * Symplectic Euler method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_SYMPLECTIC_EULER_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_SYMPLECTIC_EULER_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {


/**
 * Propagate state using Symplectic Euler.
 */
class SymplecticEulerSecondOrderODEIntegrator :
   public SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(SymplecticEulerSecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * SymplecticEulerSecondOrderODEIntegrator destructor.
    */
   virtual ~SymplecticEulerSecondOrderODEIntegrator (void);


protected:

   // Constructors and destructor.

   /**
    * SymplecticEulerSecondOrderODEIntegrator default constructor.
    * This is needed so that Trick can checkpoint/restart its integrators.
    */
   SymplecticEulerSecondOrderODEIntegrator (void);

   /**
    * SymplecticEulerSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   SymplecticEulerSecondOrderODEIntegrator (
      const SymplecticEulerSecondOrderODEIntegrator & src);

   /**
    * SymplecticEulerSecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE.
    * This constructor is used by the integrator constructor.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   SymplecticEulerSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);


   /**
    * SymplecticEulerSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided derivative function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   SymplecticEulerSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);


   /**
    * SymplecticEulerSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided position step function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   SymplecticEulerSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (SymplecticEulerSecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * posdot; /**< trick_units(--) @n
      Position derivative. */


private:

   /**
    * Not implemented.
    */
   SymplecticEulerSecondOrderODEIntegrator & operator= (
      const SymplecticEulerSecondOrderODEIntegrator &);
};


/**
 * Specialization of SymplecticEulerSecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class SymplecticEulerSimpleSecondOrderODEIntegrator :
   public SymplecticEulerSecondOrderODEIntegrator {
ER7_UTILS_MAKE_SIM_INTERFACES(SymplecticEulerSimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * SymplecticEulerSimpleSecondOrderODEIntegrator default constructor.
    */
   SymplecticEulerSimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      SymplecticEulerSecondOrderODEIntegrator()
   {}

   /**
    * SymplecticEulerSimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   SymplecticEulerSimpleSecondOrderODEIntegrator (
      const SymplecticEulerSimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      SymplecticEulerSecondOrderODEIntegrator(src)
   {}

   /**
    * SymplecticEulerSimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   SymplecticEulerSimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      SymplecticEulerSecondOrderODEIntegrator(size, controls)
   {}

   /**
    * SymplecticEulerSimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~SymplecticEulerSimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * SymplecticEulerSimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   SymplecticEulerSimpleSecondOrderODEIntegrator & operator= (
      SymplecticEulerSimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual SymplecticEulerSimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state using symplectic Euler.
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
 * Specialization of SymplecticEulerSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator :
   public SymplecticEulerSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator
    * default constructor.
    */
   SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      SymplecticEulerSecondOrderODEIntegrator()
   {}

   /**
    * SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator (
      const SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      SymplecticEulerSecondOrderODEIntegrator(src)
   {}

   /**
    * SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      SymplecticEulerSecondOrderODEIntegrator (
         position_size, velocity_size, deriv_funs, controls)
   {}

   /**
    * SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator
    * assignment operator.
    * @param[in] src  Object to be copied.
    */
   SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator & operator= (
      SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual SymplecticEulerGeneralizedDerivSecondOrderODEIntegrator *
   create_copy () const;

   /**
    * Propagate state using symplectic Euler.
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
 * Specialization of SymplecticEulerSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class SymplecticEulerGeneralizedStepSecondOrderODEIntegrator :
   public SymplecticEulerSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   SymplecticEulerGeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * SymplecticEulerGeneralizedStepSecondOrderODEIntegrator
    * default constructor.
    */
   SymplecticEulerGeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      SymplecticEulerSecondOrderODEIntegrator()
   {}

   /**
    * SymplecticEulerGeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   SymplecticEulerGeneralizedStepSecondOrderODEIntegrator (
      const SymplecticEulerGeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      SymplecticEulerSecondOrderODEIntegrator(src)
   {}

   /**
    * SymplecticEulerGeneralizedStepSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   SymplecticEulerGeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      SymplecticEulerSecondOrderODEIntegrator (
         position_size, velocity_size, step_funs, controls)
   {}

   /**
    * SymplecticEulerGeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~SymplecticEulerGeneralizedStepSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * SymplecticEulerGeneralizedStepSecondOrderODEIntegrator
    * assignment operator.
    * @param[in] src  Object to be copied.
    */
   SymplecticEulerGeneralizedStepSecondOrderODEIntegrator & operator= (
      SymplecticEulerGeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual SymplecticEulerGeneralizedStepSecondOrderODEIntegrator *
   create_copy () const;

   /**
    * Propagate state using symplectic Euler.
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

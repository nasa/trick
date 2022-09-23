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
 * Defines the class RKFehlberg45SecondOrderODEIntegrator, which integrates a
 * state comprising a zeroth derivative / first derivative pair via the
 * Runge-Kutta Fehlberg 4/5 method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RKF45_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_RKF45_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using the Runge-Kutta Fehlberg 4/5 method.
 */
class RKFehlberg45SecondOrderODEIntegrator : public SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RKFehlberg45SecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * RKFehlberg45SecondOrderODEIntegrator destructor.
    */
   virtual ~RKFehlberg45SecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * RKFehlberg45SecondOrderODEIntegrator default constructor.
    */
   RKFehlberg45SecondOrderODEIntegrator (void);

   /**
    * RKFehlberg45SecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg45SecondOrderODEIntegrator (
      const RKFehlberg45SecondOrderODEIntegrator & src);

   /**
    * RKFehlberg45SecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE, one in which generalized velocity
    * is time derivative of generalized position.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   RKFehlberg45SecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * RKFehlberg45SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position derivative function.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RKFehlberg45SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

   /**
    * RKFehlberg45SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position step function.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RKFehlberg45SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (RKFehlberg45SecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * init_pos; /**< trick_units(--) @n
      Position at the start of an integration cycle. */

   double * init_vel; /**< trick_units(--) @n
      Velocity at the start of an integration cycle. */

   double * dtheta; /**< trick_units(--) @n
      Product of delta t and weighted sum of generalized velocities. */

   double * posdot_hist[6]; /**< trick_units(--) @n
      Position derivatives at each step in the integration cycle. */

   double * veldot_hist[6]; /**< trick_units(--) @n
      Velocity derivatives at each step in the integration cycle. */


private:

   /**
    * Not implemented.
    */
   RKFehlberg45SecondOrderODEIntegrator & operator= (
      const RKFehlberg45SecondOrderODEIntegrator &);
};


/**
 * Specialization of RKFehlberg45SecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class RKFehlberg45SimpleSecondOrderODEIntegrator :
   public RKFehlberg45SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RKFehlberg45SimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKFehlberg45SimpleSecondOrderODEIntegrator default constructor.
    */
   RKFehlberg45SimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RKFehlberg45SecondOrderODEIntegrator ()
   {}

   /**
    * RKFehlberg45SimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg45SimpleSecondOrderODEIntegrator (
      const RKFehlberg45SimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RKFehlberg45SecondOrderODEIntegrator(src)
   {}

   /**
    * RKFehlberg45SimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   RKFehlberg45SimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RKFehlberg45SecondOrderODEIntegrator (size, controls)
   {}

   /**
    * RKFehlberg45SimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~RKFehlberg45SimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKFehlberg45SimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg45SimpleSecondOrderODEIntegrator & operator= (
      RKFehlberg45SimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RKFehlberg45SimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via RKF4/5 for the special case of velocity being the
    * derivative of position.
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
 * Specialization of RKFehlberg45SecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator :
   public RKFehlberg45SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RKFehlberg45SecondOrderODEIntegrator()
   {}

   /**
    * RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator (
      const RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RKFehlberg45SecondOrderODEIntegrator(src)
   {}

   /**
    * RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RKFehlberg45SecondOrderODEIntegrator (
         position_size, velocity_size, deriv_funs, controls)
   {}

   /**
    * RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator & operator= (
      RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RKFehlberg45GeneralizedDerivSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state via RKF4/5 for generalized position and velocity
    * where generalized position is advanced using the function that yields
    * the time derivative of generalized position.
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
 * Specialization of RKFehlberg45SecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class RKFehlberg45GeneralizedStepSecondOrderODEIntegrator :
   public RKFehlberg45SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   RKFehlberg45GeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKFehlberg45GeneralizedStepSecondOrderODEIntegrator default constructor.
    */
   RKFehlberg45GeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RKFehlberg45SecondOrderODEIntegrator()
   {}

   /**
    * RKFehlberg45GeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg45GeneralizedStepSecondOrderODEIntegrator (
      const RKFehlberg45GeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RKFehlberg45SecondOrderODEIntegrator(src)
   {}

   /**
    * RKFehlberg45GeneralizedStepSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RKFehlberg45GeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RKFehlberg45SecondOrderODEIntegrator (
         position_size, velocity_size, step_funs, controls)
   {}

   /**
    * RKFehlberg45GeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~RKFehlberg45GeneralizedStepSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKFehlberg45GeneralizedStepSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg45GeneralizedStepSecondOrderODEIntegrator & operator= (
      RKFehlberg45GeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RKFehlberg45GeneralizedStepSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state via RKF4/5 for generalized position and velocity
    * where generalized position is advanced using the position step function.
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

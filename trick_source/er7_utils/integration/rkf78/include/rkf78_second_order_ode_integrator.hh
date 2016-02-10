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
 * Defines the class RKFehlberg78SecondOrderODEIntegrator, which integrates a
 * state comprising a zeroth derivative / first derivative pair via the
 * Runge-Kutta Fehlberg 7/8 method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RKF78_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_RKF78_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using the Runge-Kutta Fehlberg 7/8 method.
 */
class RKFehlberg78SecondOrderODEIntegrator : public SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RKFehlberg78SecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * RKFehlberg78SecondOrderODEIntegrator destructor.
    */
   virtual ~RKFehlberg78SecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * RKFehlberg78SecondOrderODEIntegrator default constructor.
    */
   RKFehlberg78SecondOrderODEIntegrator (void);

   /**
    * RKFehlberg78SecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg78SecondOrderODEIntegrator (
      const RKFehlberg78SecondOrderODEIntegrator & src);

   /**
    * RKFehlberg78SecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE, one in which generalized velocity
    * is time derivative of generalized position.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   RKFehlberg78SecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);

   /**
    * RKFehlberg78SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position derivative function.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RKFehlberg78SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

   /**
    * RKFehlberg78SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position step function.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RKFehlberg78SecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (RKFehlberg78SecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * init_pos; /**< trick_units(--) @n
      Position at the start of an integration cycle. */

   double * init_vel; /**< trick_units(--) @n
      Velocity at the start of an integration cycle. */

   double * dtheta; /**< trick_units(--) @n
      Product of delta t and weighted sum of generalized velocities. */

   double * posdot_hist[13]; /**< trick_units(--) @n
      Position derivatives at each step in the integration cycle. */

   double * veldot_hist[13]; /**< trick_units(--) @n
      Velocity derivatives at each step in the integration cycle. */


private:

   /**
    * Not implemented.
    */
   RKFehlberg78SecondOrderODEIntegrator & operator= (
      const RKFehlberg78SecondOrderODEIntegrator &);
};


/**
 * Specialization of RKFehlberg78SecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class RKFehlberg78SimpleSecondOrderODEIntegrator :
   public RKFehlberg78SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RKFehlberg78SimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKFehlberg78SimpleSecondOrderODEIntegrator default constructor.
    */
   RKFehlberg78SimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RKFehlberg78SecondOrderODEIntegrator ()
   {}

   /**
    * RKFehlberg78SimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg78SimpleSecondOrderODEIntegrator (
      const RKFehlberg78SimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RKFehlberg78SecondOrderODEIntegrator(src)
   {}

   /**
    * RKFehlberg78SimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   RKFehlberg78SimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RKFehlberg78SecondOrderODEIntegrator (size, controls)
   {}

   /**
    * RKFehlberg78SimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~RKFehlberg78SimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKFehlberg78SimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg78SimpleSecondOrderODEIntegrator & operator= (
      RKFehlberg78SimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RKFehlberg78SimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via RKF7/8 for the special case of velocity being the
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
 * Specialization of RKFehlberg78SecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator :
   public RKFehlberg78SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RKFehlberg78SecondOrderODEIntegrator()
   {}

   /**
    * RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator (
      const RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RKFehlberg78SecondOrderODEIntegrator(src)
   {}

   /**
    * RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RKFehlberg78SecondOrderODEIntegrator (
         position_size, velocity_size, deriv_funs, controls)
   {}

   /**
    * RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator & operator= (
      RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RKFehlberg78GeneralizedDerivSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state via RKF7/8 for generalized position and velocity
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
 * Specialization of RKFehlberg78SecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class RKFehlberg78GeneralizedStepSecondOrderODEIntegrator :
   public RKFehlberg78SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   RKFehlberg78GeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKFehlberg78GeneralizedStepSecondOrderODEIntegrator default constructor.
    */
   RKFehlberg78GeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RKFehlberg78SecondOrderODEIntegrator()
   {}

   /**
    * RKFehlberg78GeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg78GeneralizedStepSecondOrderODEIntegrator (
      const RKFehlberg78GeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RKFehlberg78SecondOrderODEIntegrator(src)
   {}

   /**
    * RKFehlberg78GeneralizedStepSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RKFehlberg78GeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RKFehlberg78SecondOrderODEIntegrator (
         position_size, velocity_size, step_funs, controls)
   {}

   /**
    * RKFehlberg78GeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~RKFehlberg78GeneralizedStepSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKFehlberg78GeneralizedStepSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKFehlberg78GeneralizedStepSecondOrderODEIntegrator & operator= (
      RKFehlberg78GeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RKFehlberg78GeneralizedStepSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state via RKF7/8 for generalized position and velocity
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

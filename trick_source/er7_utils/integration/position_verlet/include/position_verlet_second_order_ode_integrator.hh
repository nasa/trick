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
 * Defines the class PositionVerletSecondOrderODEIntegrator, which integrates a
 * state comprising a zeroth derivative / first derivative pair via the
 * second order position verlet technique.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_POSITION_VERLET_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_POSITION_VERLET_TWO_STATE_INTEGRATOR_HH

// System includes

// ER7 utilities includes
#include "er7_utils/interface/include/er7_class.hh"

// Model includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"


namespace er7_utils {

/**
 * Propagate state using 2nd order Nystrom-Lear.
 */
class PositionVerletSecondOrderODEIntegrator : public SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(PositionVerletSecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * PositionVerletSecondOrderODEIntegrator destructor.
    */
   virtual ~PositionVerletSecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * PositionVerletSecondOrderODEIntegrator default constructor.
    * Notes:
    *  - This implicitly assumes the time derivative of position is velocity.
    *  - This is needed so that Trick can checkpoint/restart its integrators.
    */
   PositionVerletSecondOrderODEIntegrator (void);

   /**
    * PositionVerletSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   PositionVerletSecondOrderODEIntegrator (
      const PositionVerletSecondOrderODEIntegrator & src);

   /**
    * PositionVerletSecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE, one in which generalized velocity
    * is time derivative of generalized position.
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   PositionVerletSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls);


   /**
    * PositionVerletSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided derivative function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   PositionVerletSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

   /**
    * PositionVerletSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the provided position step function.
    * This constructor is used by the integrator constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   PositionVerletSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (PositionVerletSecondOrderODEIntegrator & other);

   using SecondOrderODEIntegrator::swap;


   // Member data.

   double * posdot; /**< trick_units(--) @n
      Derivative of generalized position. */

   double * dtheta; /**< trick_units(--) @n
      Generalized position step. */

   double * h_dtheta; /**< trick_units(--) @n
      Half of the generalized position step. */

   double * midpoint; /**< trick_units(--) @n
      Leapfrog position. */

   double * init_vel; /**< trick_units(--) @n
      Velocity at the start of the current integration interval. */

   double * prev_accel; /**< trick_units(--) @n
      Acceleration at the end of the previous integration interval. */

   bool restart; /**< trick_units(--) @n
      Make an initial half step when set. This flag is set initially and after
      a call to reset. The flag is cleared upon making that half step. */


private:

   /**
    * Not implemented.
    */
   PositionVerletSecondOrderODEIntegrator & operator= (
      const PositionVerletSecondOrderODEIntegrator &);
};


/**
 * Specialization of PositionVerletSecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class PositionVerletSimpleSecondOrderODEIntegrator :
   public PositionVerletSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(PositionVerletSimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * PositionVerletSimpleSecondOrderODEIntegrator default constructor.
    */
   PositionVerletSimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      PositionVerletSecondOrderODEIntegrator ()
   {}

   /**
    * PositionVerletSimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   PositionVerletSimpleSecondOrderODEIntegrator (
      const PositionVerletSimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      PositionVerletSecondOrderODEIntegrator(src)
   {}

   /**
    * PositionVerletSimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   PositionVerletSimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      PositionVerletSecondOrderODEIntegrator (size, controls)
   {}

   /**
    * PositionVerletSimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~PositionVerletSimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * PositionVerletSimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   PositionVerletSimpleSecondOrderODEIntegrator & operator= (
      PositionVerletSimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this'.
    * @return Clone of 'this'.
    */
   virtual PositionVerletSimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state using Nystrom Lear 2.
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
 * Specialization of PositionVerletSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class PositionVerletGeneralizedDerivSecondOrderODEIntegrator :
   public PositionVerletSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   PositionVerletGeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * PositionVerletGeneralizedDerivSecondOrderODEIntegrator
    * default constructor.
    */
   PositionVerletGeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      PositionVerletSecondOrderODEIntegrator ()
   {}

   /**
    * PositionVerletGeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   PositionVerletGeneralizedDerivSecondOrderODEIntegrator (
      const PositionVerletGeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      PositionVerletSecondOrderODEIntegrator (src)
   {}

   /**
    * PositionVerletGeneralizedDerivSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   PositionVerletGeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      PositionVerletSecondOrderODEIntegrator (position_size, velocity_size,
                                            deriv_funs, controls)
   {}

   /**
    * PositionVerletGeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~PositionVerletGeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * PositionVerletGeneralizedDerivSecondOrderODEIntegrator
    * assignment operator.
    * @param[in] src  Object to be copied.
    */
   PositionVerletGeneralizedDerivSecondOrderODEIntegrator & operator= (
      PositionVerletGeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual PositionVerletGeneralizedDerivSecondOrderODEIntegrator *
   create_copy () const;

   /**
    * Propagate state using Nystrom Lear 2.
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
 * Specialization of PositionVerletSecondOrderODEIntegrator for the case of
 * the generalized position being constrained to lie on a manifold.
 * The function compute_position_step makes a non-linear position step based
 * generalized position, generalized velocity, and the time step.
 */
class PositionVerletGeneralizedStepSecondOrderODEIntegrator :
   public PositionVerletSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(
   PositionVerletGeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * PositionVerletGeneralizedStepSecondOrderODEIntegrator default constructor.
    */
   PositionVerletGeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      PositionVerletSecondOrderODEIntegrator ()
   {}

   /**
    * PositionVerletGeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   PositionVerletGeneralizedStepSecondOrderODEIntegrator (
      const PositionVerletGeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      PositionVerletSecondOrderODEIntegrator (src)
   {}

   /**
    * PositionVerletGeneralizedStepSecondOrderODEIntegrator
    * non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls      Integration controls
    */
   PositionVerletGeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      PositionVerletSecondOrderODEIntegrator (position_size, velocity_size,
                                            step_funs, controls)
   {}

   /**
    * PositionVerletGeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~PositionVerletGeneralizedStepSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * PositionVerletGeneralizedStepSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   PositionVerletGeneralizedStepSecondOrderODEIntegrator & operator= (
      PositionVerletGeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual PositionVerletGeneralizedStepSecondOrderODEIntegrator *
   create_copy () const;

   /**
    * Reset a Nystrom Lear 2 integrator.
    */
   virtual void reset_integrator (void)
   {
      restart = true;
   }


   /**
    * Propagate state using Nystrom Lear 2.
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

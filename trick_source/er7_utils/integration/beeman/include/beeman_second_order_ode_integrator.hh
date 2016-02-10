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
 * Defines the class BeemanSecondOrderODEIntegrator, which integrates a state
 * comprising a zeroth derivative / first derivative pair via Beeman's method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_BEEMAN_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_BEEMAN_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/priming_second_order_ode_integrator.hh"


namespace er7_utils {

class IntegratorConstructor;

/**
 * Advance state using Beeman's method.
 * This includes
 *   - Initial priming of the recent history of state derivatives,
 *   - Using Beeman's method once primed, and
 *   - Clearing the history when needed (e.g., time change or state change).
 */
class BeemanSecondOrderODEIntegrator : public PrimingSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(BeemanSecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * BeemanSecondOrderODEIntegrator destructor.
    */
   virtual ~BeemanSecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * BeemanSecondOrderODEIntegrator default constructor.
    * Notes:
    *  - This implicitly assumes the time derivative of position is velocity.
    *  - This is needed so that Trick can checkpoint/restart its integrators.
    */
   BeemanSecondOrderODEIntegrator (void);

   /**
    * BeemanSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   BeemanSecondOrderODEIntegrator (const BeemanSecondOrderODEIntegrator & src);

   /**
    * BeemanSecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE.
    * This constructor is used by the integrator constructor.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   BeemanSecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int size,
      IntegrationControls & controls);

   /**
    * BeemanSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position derivative function.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     deriv_funs          Position derivative functions container
    * @param[in,out] controls            Integration controls
    */
   BeemanSecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

// The implementation of BeemanGeneralizedStepSecondOrderODEIntegrator
// is not ready for prime time.
#if 0
   /**
    * BeemanSecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position step function.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     step_funs           Position step functions container
    * @param[in,out] controls            Integration controls
    */
   BeemanSecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls);
#endif


   // Member functions.

   /**
    * Non-throwing swap.
    * @param other  Item with which contents are to be swapped.
    */
   void swap (BeemanSecondOrderODEIntegrator & other);

   using PrimingSecondOrderODEIntegrator::swap;


   // Member data.

   double * init_vel; /**< trick_units(--) @n
      Velocity at the start of an integration cycle. */

   double * mean_vel; /**< trick_units(--) @n
      Velocity used to update position. */

   double * init_acc; /**< trick_units(--) @n
      Acceleration at the start of an integration cycle. */

   double * prev_acc; /**< trick_units(--) @n
      Acceleration at the start of the previous integration cycle. */

   double * posdot; /**< trick_units(--) @n
      Position derivative at the current time step. */

   double * posddot; /**< trick_units(--) @n
      Position 2nd derivative at the current time step. */

   double * init_posddot; /**< trick_units(--) @n
      Position 2nd derivative at the start of an integration cycle. */

   double * prev_posddot; /**< trick_units(--) @n
      Position 2nd derivative at the start of the previous integration cycle. */


private:

   /**
    * Not implemented.
    */
   BeemanSecondOrderODEIntegrator & operator= (
      const BeemanSecondOrderODEIntegrator &);
};


/**
 * Specialization of BeemanSecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class BeemanSimpleSecondOrderODEIntegrator :
   public BeemanSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(BeemanSimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * BeemanSimpleSecondOrderODEIntegrator default constructor.
    */
   BeemanSimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      BeemanSecondOrderODEIntegrator()
   {}

   /**
    * BeemanSimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   BeemanSimpleSecondOrderODEIntegrator (
      const BeemanSimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      BeemanSecondOrderODEIntegrator(src)
   {}

   /**
    * BeemanSimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     size                Size of the position, velocity vectors
    * @param[in,out] controls_in         Integration controls
    */
   BeemanSimpleSecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int size,
      IntegrationControls & controls_in)
   :
      Er7UtilsDeletable(),
      BeemanSecondOrderODEIntegrator(primer_constructor, size, controls_in)
   {}

   /**
    * BeemanSimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~BeemanSimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * BeemanSimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   BeemanSimpleSecondOrderODEIntegrator & operator= (
      BeemanSimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' BeemanSimpleSecondOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual BeemanSimpleSecondOrderODEIntegrator * create_copy () const;


protected:

   /**
    * Save derivatives, called during priming.
    * @param countdown  The priming countdown.
    * @param accel      Time derivative of the generalized velocity.
    * @param velocity   Generalized velocity vector.
    * @param position   Generalized position vector.
    */
   virtual void technique_save_derivatives (
      int countdown,
      double const * ER7_UTILS_RESTRICT accel,
      double const * ER7_UTILS_RESTRICT velocity,
      double const * ER7_UTILS_RESTRICT position);

   /**
    * Propagate state using Beeman's method, excluding priming.
    * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in]     accel         Time derivative of the generalized velocity.
    * @param[in,out] velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult technique_integrate (
      double dyn_dt,
      unsigned int target_stage,
      double const * ER7_UTILS_RESTRICT accel,
      double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position);
};


/**
 * Specialization of BeemanSecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class BeemanGeneralizedDerivSecondOrderODEIntegrator :
   public BeemanSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(BeemanGeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * BeemanGeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   BeemanGeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      BeemanSecondOrderODEIntegrator()
   {}

   /**
    * BeemanGeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   BeemanGeneralizedDerivSecondOrderODEIntegrator (
      const BeemanGeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      BeemanSecondOrderODEIntegrator(src)
   {}

   /**
    * BeemanGeneralizedDerivSecondOrderODEIntegrator non-default constructor.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     deriv_funs          Position derivative functions container
    * @param[in,out] controls_in         Integration controls
    */
   BeemanGeneralizedDerivSecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls_in)
   :
      Er7UtilsDeletable (),
      BeemanSecondOrderODEIntegrator (primer_constructor,
                                    position_size, velocity_size,
                                    deriv_funs, controls_in)
   {}

   /**
    * BeemanGeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~BeemanGeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * BeemanGeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   BeemanGeneralizedDerivSecondOrderODEIntegrator & operator= (
      BeemanGeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual BeemanGeneralizedDerivSecondOrderODEIntegrator * create_copy ()
   const;


protected:

   /**
    * Save derivatives, called during priming.
    * @param countdown  The priming countdown.
    * @param accel      Time derivative of the generalized velocity.
    * @param velocity   Generalized velocity vector.
    * @param position   Generalized position vector.
    */
   virtual void technique_save_derivatives (
      int countdown,
      double const * ER7_UTILS_RESTRICT accel,
      double const * ER7_UTILS_RESTRICT velocity,
      double const * ER7_UTILS_RESTRICT position);

   /**
    * Propagate state using Beeman's method, excluding priming.
    * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in]     accel         Time derivative of the generalized velocity.
    * @param[in,out] velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult technique_integrate (
      double dyn_dt,
      unsigned int target_stage,
      double const * ER7_UTILS_RESTRICT accel,
      double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position);
};


// This old implementation of BeemanGeneralizedStepSecondOrderODEIntegrator
// is not ready for prime time.
#if 0
/**
 * Specialization of BeemanSecondOrderODEIntegrator for the case of
 * the generalized position being constrained to lie on a manifold.
 * The generalized position step function makes a non-linear position step that
 * keeps the generalized position on the manifold.
 */
class BeemanGeneralizedStepSecondOrderODEIntegrator :
   public BeemanSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(BeemanGeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   // Default constructor, exists because the memory manager needs one.
   BeemanGeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      BeemanSecondOrderODEIntegrator()
   {}

   /**
    * BeemanGeneralizedStepSecondOrderODEIntegrator non-default constructor.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     step_funs           Position step functions container
    * @param[in,out] controls_in         Integration controls
    */
   BeemanGeneralizedStepSecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls_in)
   :
      Er7UtilsDeletable(),
      BeemanSecondOrderODEIntegrator (primer_constructor,
                                    position_size, velocity_size,
                                    step_funs, controls_in)
   {}

   /**
    * BeemanGeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~BeemanGeneralizedStepSecondOrderODEIntegrator (void)
   {}


protected:

   /**
    * Propagate state using Beeman's method, excluding priming.
    * @param[in]     dyn_dt        Dynamic time step, in dynamic time seconds.
    * @param[in]     target_stage  The stage of the integration process
    *                              that the integrator should try to attain.
    * @param[in]     accel         Time derivative of the generalized velocity.
    * @param[in,out] velocity      Generalized velocity vector.
    * @param[in,out] position      Generalized position vector.
    *
    * @return The status (time advance, pass/fail status) of the integration.
    */
   virtual IntegratorResult technique_integrate (
      double dyn_dt,
      unsigned int target_stage,
      double const * ER7_UTILS_RESTRICT accel,
      double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position);


private:

   /**
    * Not implemented.
    */
   BeemanGeneralizedStepSecondOrderODEIntegrator (
      const BeemanGeneralizedStepSecondOrderODEIntegrator &);

   /**
    * Not implemented.
    */
   BeemanGeneralizedStepSecondOrderODEIntegrator & operator= (
      const BeemanGeneralizedStepSecondOrderODEIntegrator &);
};
#endif

}


#ifdef ER7_UTILS_NEED_AUX_INCLUDES
#include "er7_utils/integration/core/include/integration_controls.hh"
#endif


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

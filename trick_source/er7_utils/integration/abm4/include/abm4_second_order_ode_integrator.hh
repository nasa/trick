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
 * Defines the class ABM4SecondOrderODEIntegrator, which integrates a state
 * comprising a zeroth derivative / first derivative pair via the
 * fourth order Adams Bashforth Moulton method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_ABM4_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_ABM4_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/priming_second_order_ode_integrator.hh"


namespace er7_utils {

class IntegratorConstructor;

/**
 * Advance state using the fourth order Adams-Bashforth-Moulton method.
 * This includes
 *   - Initial priming of the recent history of state derivatives,
 *   - Using the ABM-4 method once primed, and
 *   - Clearing the history when needed (e.g., time change or state change).
 */
class ABM4SecondOrderODEIntegrator : public PrimingSecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(ABM4SecondOrderODEIntegrator)


   // Note:
   //  This is an abstract class.
   //  - The constructors for this class are protected.
   //  - The assignment operator for this class is private / unimplemented.


public:

   /**
    * ABM4SecondOrderODEIntegrator destructor.
    */
   virtual ~ABM4SecondOrderODEIntegrator (void);


protected:

   // Constructors.

   /**
    * ABM4SecondOrderODEIntegrator default constructor.
    */
   ABM4SecondOrderODEIntegrator (void);

   /**
    * ABM4SecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   ABM4SecondOrderODEIntegrator (const ABM4SecondOrderODEIntegrator & src);

   /**
    * ABM4SecondOrderODEIntegrator non-default constructor
    * for a simple second order ODE.
    * This constructor is used by the integrator constructor.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     size                State size
    * @param[in,out] controls            Integration controls
    */
   ABM4SecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int size,
      IntegrationControls & controls);

   /**
    * ABM4SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position derivative function.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     deriv_funs          Position derivative functions container
    * @param[in,out] controls            Integration controls
    */
   ABM4SecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls);

// The current implementation of ABM4GeneralizedStepSecondOrderODEIntegrator
// is not ready for prime time.
#if 0
   /**
    * ABM4SecondOrderODEIntegrator non-default constructor
    * for a generalized second order ODE in which position is advanced
    * using the position step function.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     step_funs           Position step functions container
    * @param[in,out] controls            Integration controls
    */
   ABM4SecondOrderODEIntegrator (
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
   void swap (ABM4SecondOrderODEIntegrator & other);

   using PrimingSecondOrderODEIntegrator::swap;


   // Member data.

   double * init_pos; /**< trick_units(--) @n
      Position at the start of an integration cycle. */

   double * init_vel; /**< trick_units(--) @n
      Velocity at the start of an integration cycle. */

   double * posdot; /**< trick_units(--) @n
      Position derivative at the current time step. */

#if 0
   double * dtheta; /**< trick_units(--) @n
      Product of delta t and weighted sum of generalized velocities. */
#endif

   double * posdot_hist[4]; /**< trick_units(--) @n
      Position derivatives at each step in the integration cycle. */

   double * posdot_hist_anchor; /**< trick_units(--) @n
   Cached pointer to the data needed for deletion after PDH is rotated */

   double * veldot_hist[4]; /**< trick_units(--) @n
      Velocity derivatives at each step in the integration cycle. */

   double * veldot_hist_anchor; /**< trick_units(--) @n
   Cached pointer to the data needed for deletion after VDH is rotated */


private:

   /**
    * Not implemented.
    */
   ABM4SecondOrderODEIntegrator & operator= (
      const ABM4SecondOrderODEIntegrator &);
};


/**
 * Specialization of ABM4SecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class ABM4SimpleSecondOrderODEIntegrator :
   public ABM4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(ABM4SimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * ABM4SimpleSecondOrderODEIntegrator default constructor.
    */
   ABM4SimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      ABM4SecondOrderODEIntegrator()
   {}

   /**
    * ABM4SimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   ABM4SimpleSecondOrderODEIntegrator (
      const ABM4SimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      ABM4SecondOrderODEIntegrator(src)
   {}

   /**
    * ABM4SimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     size                Size of the position, velocity vectors
    * @param[in,out] controls_in         Integration controls
    */
   ABM4SimpleSecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int size,
      IntegrationControls & controls_in)
   :
      Er7UtilsDeletable(),
      ABM4SecondOrderODEIntegrator(primer_constructor, size, controls_in)
   {}

   /**
    * ABM4SimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~ABM4SimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * ABM4SimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   ABM4SimpleSecondOrderODEIntegrator & operator= (
      ABM4SimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' ABM4SimpleSecondOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual ABM4SimpleSecondOrderODEIntegrator * create_copy () const;


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
    * Propagate state using fourth order Adams Bashforth Moulton,
    * excluding priming.
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
 * Specialization of ABM4SecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class ABM4GeneralizedDerivSecondOrderODEIntegrator :
   public ABM4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(ABM4GeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * ABM4GeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   ABM4GeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      ABM4SecondOrderODEIntegrator()
   {}

   /**
    * ABM4GeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   ABM4GeneralizedDerivSecondOrderODEIntegrator (
      const ABM4GeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      ABM4SecondOrderODEIntegrator(src)
   {}

   /**
    * ABM4GeneralizedDerivSecondOrderODEIntegrator non-default constructor.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     deriv_funs          Position derivative functions container
    * @param[in,out] controls_in         Integration controls
    */
   ABM4GeneralizedDerivSecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls_in)
   :
      Er7UtilsDeletable(),
      ABM4SecondOrderODEIntegrator (primer_constructor,
                                    position_size, velocity_size,
                                    deriv_funs, controls_in)
   {}

   /**
    * ABM4GeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~ABM4GeneralizedDerivSecondOrderODEIntegrator (void)
   {}

   /**
    * ABM4GeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   ABM4GeneralizedDerivSecondOrderODEIntegrator & operator= (
      ABM4GeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' ABM4GeneralizedDerivSecondOrderODEIntegrator.
    * @return Clone of 'this'.
    */
   virtual ABM4GeneralizedDerivSecondOrderODEIntegrator * create_copy () const;


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
    * Propagate state using fourth order Adams Bashforth Moulton,
    * excluding priming.
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


// The current implementation of ABM4GeneralizedStepSecondOrderODEIntegrator
// is not ready for prime time.
#if 0
/**
 * Specialization of ABM4SecondOrderODEIntegrator for the case of
 * the generalized position being constrained to lie on a manifold.
 * The generalized position step function makes a non-linear position step that
 * keeps the generalized position on the manifold.
 */
class ABM4GeneralizedStepSecondOrderODEIntegrator :
   public ABM4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(ABM4GeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   // Default constructor, exists because the memory manager needs one.
   ABM4GeneralizedStepSecondOrderODEIntegrator (void)
   :
      ABM4SecondOrderODEIntegrator()
   {}

   /**
    * ABM4GeneralizedStepSecondOrderODEIntegrator non-default constructor.
    * @param[in]     primer_constructor  Constructor that creates the primer
    * @param[in]     position_size       Size of the generalized position
    * @param[in]     velocity_size       Size of the generalized velocity
    * @param[in]     step_funs           Position step functions container
    * @param[in,out] controls_in         Integration controls
    */
   ABM4GeneralizedStepSecondOrderODEIntegrator (
      const IntegratorConstructor & primer_constructor,
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls_in)
   :
      ABM4SecondOrderODEIntegrator (primer_constructor,
                                    position_size, velocity_size,
                                    step_funs, controls_in)
   {}

   /**
    * ABM4GeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~ABM4GeneralizedStepSecondOrderODEIntegrator (void)
   {}


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
    * Propagate state using fourth order Adams Bashforth Moulton,
    * excluding priming.
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
   ABM4GeneralizedStepSecondOrderODEIntegrator (
      const ABM4GeneralizedStepSecondOrderODEIntegrator &);

   /**
    * Not implemented.
    */
   ABM4GeneralizedStepSecondOrderODEIntegrator & operator= (
      const ABM4GeneralizedStepSecondOrderODEIntegrator &);
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

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
 * Defines the class RK4SecondOrderODEIntegrator, which integrates a state
 * comprising a zeroth derivative / first derivative pair via the RK4 method.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RK4_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_RK4_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/core/include/second_order_ode_integrator.hh"

// Local includes
#include "rk4_second_order_ode_integrator_base.hh"


namespace er7_utils {


/**
 * Specialization of RK4SecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class RK4SimpleSecondOrderODEIntegrator :
   public RK4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RK4SimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK4SimpleSecondOrderODEIntegrator default constructor.
    */
   RK4SimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      RK4SecondOrderODEIntegrator()
   {}

   /**
    * RK4SimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK4SimpleSecondOrderODEIntegrator (
      const RK4SimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      RK4SecondOrderODEIntegrator(src)
   {}

   /**
    * RK4SimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   RK4SimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      RK4SecondOrderODEIntegrator(size, controls)
   {}

   /**
    * RK4SimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~RK4SimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RK4SimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RK4SimpleSecondOrderODEIntegrator & operator= (
      RK4SimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' RK4SimpleSecondOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual RK4SimpleSecondOrderODEIntegrator * create_copy () const;


   /**
    * Propagate state via RK4 for the special case of velocity being the
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
 * Specialization of RK4SecondOrderODEIntegrator for the case of
 * the time derivative of generalized position being some function of
 * the generalized position and generalized velocity.
 */
class RK4GeneralizedDerivSecondOrderODEIntegrator :
   public RK4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RK4GeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK4GeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   RK4GeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      RK4SecondOrderODEIntegrator()
   {}

   /**
    * RK4GeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK4GeneralizedDerivSecondOrderODEIntegrator (
      const RK4GeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      RK4SecondOrderODEIntegrator(src)
   {}

   /**
    * RK4GeneralizedDerivSecondOrderODEIntegrator non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RK4GeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      RK4SecondOrderODEIntegrator (position_size, velocity_size,
                                   deriv_funs, controls)
   {}

   /**
    * RK4GeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~RK4GeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RK4GeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RK4GeneralizedDerivSecondOrderODEIntegrator & operator= (
      RK4GeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RK4GeneralizedDerivSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via RK4 for generalized position and generalized velocity
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
 * Specialization of RK4SecondOrderODEIntegrator for the case of
 * the generalized position being constrained to lie on a manifold.
 * The generalized position step function makes a non-linear position step that
 * keeps the generalized position on the manifold.
 */
class RK4GeneralizedStepSecondOrderODEIntegrator :
   public RK4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RK4GeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RK4GeneralizedStepSecondOrderODEIntegrator default constructor.
    */
   RK4GeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable(),
      RK4SecondOrderODEIntegrator()
   {}

   /**
    * RK4GeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RK4GeneralizedStepSecondOrderODEIntegrator (
      const RK4GeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable(),
      RK4SecondOrderODEIntegrator(src)
   {}

   /**
    * RK4GeneralizedStepSecondOrderODEIntegrator non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RK4GeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable(),
      RK4SecondOrderODEIntegrator (position_size, velocity_size,
                                   step_funs, controls)
   {}

   /**
    * RK4GeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~RK4GeneralizedStepSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RK4GeneralizedStepSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RK4GeneralizedStepSecondOrderODEIntegrator & operator= (
      RK4GeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RK4GeneralizedStepSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via RK4 for generalized position and generalized velocity
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

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
 * Defines the three fourth order Runge Kutta Gill classes that derive from
 * RK4SecondOrderODEIntegrator:
 * - RKGill4SimpleSecondOrderODEIntegrator,
 * - RKGill4GeneralizedDerivSecondOrderODEIntegrator, and
 * - RKGill4GeneralizedStepSecondOrderODEIntegrator.
 *
 * The class RK4SecondOrderODEIntegrator is used as a base because fourth order
 * Runge Kutta Gill shares so much in common with canonical fourth order
 * Runge Kutta that there is no need for a new base class.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_RKGILL4_TWO_STATE_INTEGRATOR_HH
#define ER7_UTILS_RKGILL4_TWO_STATE_INTEGRATOR_HH

// System includes

// Interface includes
#include "er7_utils/interface/include/er7_class.hh"

// Integration includes
#include "er7_utils/integration/rk4/include/rk4_second_order_ode_integrator_base.hh"


namespace er7_utils {

/**
 * Specialization of RK4SecondOrderODEIntegrator for the case of
 * generalized velocity being the time derivative of generalized position.
 */
class RKGill4SimpleSecondOrderODEIntegrator :
   public RK4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RKGill4SimpleSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKGill4SimpleSecondOrderODEIntegrator default constructor.
    */
   RKGill4SimpleSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RK4SecondOrderODEIntegrator ()
   {}

   /**
    * RKGill4SimpleSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKGill4SimpleSecondOrderODEIntegrator (
      const RKGill4SimpleSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RK4SecondOrderODEIntegrator (src)
   {}

   /**
    * RKGill4SimpleSecondOrderODEIntegrator non-default constructor.
    * @param[in]     size      Size of the position and velocity vectors
    * @param[in,out] controls  Integration controls
    */
   RKGill4SimpleSecondOrderODEIntegrator (
      unsigned int size,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RK4SecondOrderODEIntegrator (size, controls)
   {}

   /**
    * RKGill4SimpleSecondOrderODEIntegrator destructor.
    */
   virtual ~RKGill4SimpleSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKGill4SimpleSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKGill4SimpleSecondOrderODEIntegrator & operator= (
      RKGill4SimpleSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' RKGill4SimpleSecondOrderODEIntegrator object.
    * @return Clone of 'this'.
    */
   virtual RKGill4SimpleSecondOrderODEIntegrator * create_copy () const;

   /**
    * Propagate state via RKGill4 for the special case of velocity being the
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
class RKGill4GeneralizedDerivSecondOrderODEIntegrator :
   public RK4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RKGill4GeneralizedDerivSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKGill4GeneralizedDerivSecondOrderODEIntegrator default constructor.
    */
   RKGill4GeneralizedDerivSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RK4SecondOrderODEIntegrator ()
   {}

   /**
    * RKGill4GeneralizedDerivSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKGill4GeneralizedDerivSecondOrderODEIntegrator (
      const RKGill4GeneralizedDerivSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RK4SecondOrderODEIntegrator (src)
   {}

   /**
    * RKGill4GeneralizedDerivSecondOrderODEIntegrator non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     deriv_funs     Position derivative functions container
    * @param[in,out] controls       Integration controls
    */
   RKGill4GeneralizedDerivSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionDerivativeFunctions & deriv_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RK4SecondOrderODEIntegrator (
         position_size, velocity_size, deriv_funs, controls)
   {}

   /**
    * RKGill4GeneralizedDerivSecondOrderODEIntegrator destructor.
    */
   virtual ~RKGill4GeneralizedDerivSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKGill4GeneralizedDerivSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKGill4GeneralizedDerivSecondOrderODEIntegrator & operator= (
      RKGill4GeneralizedDerivSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RKGill4GeneralizedDerivSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state via RKGill4 for generalized position and velocity
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
class RKGill4GeneralizedStepSecondOrderODEIntegrator :
   public RK4SecondOrderODEIntegrator {

ER7_UTILS_MAKE_SIM_INTERFACES(RKGill4GeneralizedStepSecondOrderODEIntegrator)

public:

   // Constructors and destructor.

   /**
    * RKGill4GeneralizedStepSecondOrderODEIntegrator default constructor.
    */
   RKGill4GeneralizedStepSecondOrderODEIntegrator (void)
   :
      Er7UtilsDeletable (),
      RK4SecondOrderODEIntegrator ()
   {}

   /**
    * RKGill4GeneralizedStepSecondOrderODEIntegrator copy constructor.
    * @param[in] src  Object to be copied.
    */
   RKGill4GeneralizedStepSecondOrderODEIntegrator (
      const RKGill4GeneralizedStepSecondOrderODEIntegrator & src)
   :
      Er7UtilsDeletable (),
      RK4SecondOrderODEIntegrator (src)
   {}

   /**
    * RKGill4GeneralizedStepSecondOrderODEIntegrator non-default constructor.
    * @param[in]     position_size  Size of the generalized position
    * @param[in]     velocity_size  Size of the generalized velocity
    * @param[in]     step_funs      Position step functions container
    * @param[in,out] controls       Integration controls
    */
   RKGill4GeneralizedStepSecondOrderODEIntegrator (
      unsigned int position_size,
      unsigned int velocity_size,
      const GeneralizedPositionStepFunctions & step_funs,
      IntegrationControls & controls)
   :
      Er7UtilsDeletable (),
      RK4SecondOrderODEIntegrator (
         position_size, velocity_size, step_funs, controls)
   {}

   /**
    * RKGill4GeneralizedStepSecondOrderODEIntegrator destructor.
    */
   virtual ~RKGill4GeneralizedStepSecondOrderODEIntegrator (void)
   {}


   // Member functions.

   /**
    * RKGill4GeneralizedStepSecondOrderODEIntegrator assignment operator.
    * @param[in] src  Object to be copied.
    */
   RKGill4GeneralizedStepSecondOrderODEIntegrator & operator= (
      RKGill4GeneralizedStepSecondOrderODEIntegrator src)
   {
      swap (src);
      return *this;
   }

   /**
    * Create a copy of 'this' object.
    * @return Clone of 'this'.
    */
   virtual RKGill4GeneralizedStepSecondOrderODEIntegrator * create_copy ()
   const;

   /**
    * Propagate state via RKGill4 for generalized position and velocity
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

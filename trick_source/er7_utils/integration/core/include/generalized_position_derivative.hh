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
 * Defines the classes GeneralizedPositionDerivativeFunctions and
 * GeneralizedPositionStepFunctions, which define and encapsulate
 * pointers to various functions needed to support integration of generalized
 * position.
 */

/*
Purpose: ()
*/


#ifndef ER7_UTILS_GENERALIZED_POSITION_DERIVATIVE_HH
#define ER7_UTILS_GENERALIZED_POSITION_DERIVATIVE_HH


// Interface includes
#include "er7_utils/interface/include/config.hh"


namespace er7_utils {


/**
 * Encapsulates pointers to functions that calculate the first and
 * second time derivatives of generalized position.
 */
class GeneralizedPositionDerivativeFunctions {
public:

   // Types

   /**
    * Pointer to a function that computes the time derivative of the generalized
    * position as a function of generalized position and generalized velocity.
    *
    * @param[in]  position             Generalized position vector
    * @param[in]  velocity             Generalized velocity vector
    * @param[out] position_derivative  Time derivative of generalized position
    */
   typedef void (* FirstDerivative) (
      const double * ER7_UTILS_RESTRICT position,
      const double * ER7_UTILS_RESTRICT velocity,
      double * ER7_UTILS_RESTRICT position_derivative);

   /**
    * Pointer to a function that computes the second time derivative of
    * generalized position as a function of generalized position, generalized
    * velocity, and generalized acceleration (the time derivative of generalized
    * velocity).
    *
    * @param[in]  position                 Generalized position vector
    * @param[in]  velocity                 Generalized velocity vector
    * @param[in]  acceleration             Generalized velocity vector
    * @param[out] position_2nd_derivative  Generalized position 2nd derivative
    */
   typedef void (* SecondDerivative) (
      const double * ER7_UTILS_RESTRICT position,
      const double * ER7_UTILS_RESTRICT velocity,
      const double * ER7_UTILS_RESTRICT acceleration,
      double * ER7_UTILS_RESTRICT position_2nd_derivative);


   // Constructors and destructor
   // Note: The copy constructor and assignment operator are implicitly defined.

   /**
    * Default constructor.
    */
   GeneralizedPositionDerivativeFunctions ()
   :
      first_deriv_fun (0),
      second_deriv_fun (0)
   { }


   /**
    * Non-default constructor.
    * @param first_deriv_fun_in   Generalized position time derivative function
    * @param second_deriv_fun_in  Generalized position 2nd time deriv function
    */
   GeneralizedPositionDerivativeFunctions (
      FirstDerivative first_deriv_fun_in,
      SecondDerivative second_deriv_fun_in)
   :
      first_deriv_fun (first_deriv_fun_in),
      second_deriv_fun (second_deriv_fun_in)
   { }


   /**
    * Destructor.
    */
   virtual ~GeneralizedPositionDerivativeFunctions () {}


   // Member data

   /**
    * Function that computes the first time derivative of
    * generalized position.
    */
   FirstDerivative first_deriv_fun; //!< trick_io(**)

   /**
    * Function that computes the second time derivative of
    * generalized position.
    */
   SecondDerivative second_deriv_fun; //!< trick_io(**)

};


/**
 * Encapsulates pointers to functions that are needed by Lie group integrators.
 */
class GeneralizedPositionStepFunctions {
public:

   // Types


   /**
    * Pointer to a function that advances the generalized position as a function
    * of generalized position, generalized velocity, and delta t.
    *
    * @note
    * A viable candidate must be able to handle the output generalized position
    * pointing to the same place as the input generalized position and the
    * output exponential map pointing to the same place as the input map.
    *
    * @param[in]  position          Generalized position vector.
    * @param[in]  dtheta            Product of time and generalized velocity.
    * @param[out] updated_position  Generalized position after the time step.
    */
   typedef void (* ExpMapPositionStep) (
      const double * position,
      const double * dtheta,
      double * updated_position);


   /**
    * Pointer to a function that transforms a generalized velocity per
    * the dexp inverse operator.
    *
    * @note
    * A viable candidate must be able to handle the output generalized position
    * pointing to the same place as the input generalized position.
    *
    * @param[in]  subject_velocity  Generalized velocity vector is to be
    *                               transformed.
    * @param[in]  dtheta            dtheta vector computed during the previous
    *                               integration step.
    * @param[out] xform_velocity    Generalized position after the time step.
    */
   typedef void (* DexpinvTransformVelocity) (
      const double * subject_velocity,
      const double * dtheta,
      double * xform_velocity);


   // Constructors and destructor
   // Note: The copy constructor and assignment operator are implicitly defined.

   /**
    * Default constructor.
    */
   GeneralizedPositionStepFunctions ()
   :
      expmap_step_fun (0),
      dexpinv_xform_fun (0)
   { }


   /**
    * Non-default constructor.
    * @param expmap_step_fun_in    Exponential map step function
    * @param dexpinv_xform_fun_in  Velocity transformation function
    */
   GeneralizedPositionStepFunctions (
      ExpMapPositionStep expmap_step_fun_in,
      DexpinvTransformVelocity dexpinv_xform_fun_in)
   :
      expmap_step_fun (expmap_step_fun_in),
      dexpinv_xform_fun (dexpinv_xform_fun_in)
   { }


   /**
    * Destructor.
    */
   virtual ~GeneralizedPositionStepFunctions () {}


   // Member data

   ExpMapPositionStep expmap_step_fun; /**< trick_io(**) @n
      Exponential map step function. */

   DexpinvTransformVelocity dexpinv_xform_fun; /**< trick_io(**) @n
      Velocity transformation function. */

};

}

#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

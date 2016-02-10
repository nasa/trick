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
 * Defines the classes
 * LeftQuaternionGeneralizedPositionDerivativeFunctions,
 * LeftQuaternionGeneralizedPositionStepFunctions, and
 * LeftQuaternionGeneralizedPositionFunctions.
 */

/*
Purpose: ()
*/

#ifndef ER7_UTILS_LEFT_QUATERNION_FUNCTIONS_HH
#define ER7_UTILS_LEFT_QUATERNION_FUNCTIONS_HH


// Integration includes
#include "generalized_position_derivative.hh"


namespace er7_utils {


/**
 * Specialization of GeneralizedPositionDerivativeFunctions for the
 * rotation group SO(3) in which
 *  - Generalized position is represented by a parent to body
 *    left transformation quaternion in which the real scalar part is the
 *    initial (zeroth) element of the quaternion and the imaginary vectorial
 *    part comprises the remaining three elements, and
 *  - Generalized velocity is represented by the angular velocity of
 *    the body frame with respect to the parent frame but expressed
 *    in body frame coordinates.
 */
class LeftQuaternionGeneralizedPositionDerivativeFunctions :
   public GeneralizedPositionDerivativeFunctions {
public:

   /**
    * LeftQuaternionGeneralizedPositionDerivativeFunctions default constructor.
    */
   LeftQuaternionGeneralizedPositionDerivativeFunctions()
   :
      GeneralizedPositionDerivativeFunctions (derivative, second_derivative)
   { }


   /**
    * Compute the time derivative of an inertial to body left transformation
    * quaternion as a function of the quaternion and angular velocity.
    *
    * @param[in]  quaternion
    *             Inertial to body-fixed left transformation quaternion.
    * @param[in]  angular_vel
    *             Angular velocity in radians per second of the body frame wrt
    *             inertial, expressed in body-fixed coordinates.
    * @param[out] quaternion_deriv
    *             Time derivative of the input quaternion.
    */
   static void derivative (
      const double * ER7_UTILS_RESTRICT quaternion,
      const double * ER7_UTILS_RESTRICT angular_vel,
      double * ER7_UTILS_RESTRICT quaternion_deriv);


   /**
    * Compute the second time derivative of an inertial to body left
    * transformation quaternion as a function of the quaternion, angular
    * velocity, and angular acceleration.
    *
    * @param[in]  quaternion
    *             Inertial to body-fixed left transformation quaternion.
    * @param[in]  angular_vel
    *             Angular velocity in radians per second of the body frame wrt
    *             inertial, expressed in body-fixed coordinates.
    * @param[in]  angular_acc
    *             Angular acceleration in radians per second^2 of the body frame
    *             wrt inertial, expressed in body-fixed coordinates.
    * @param[out] quaternion_second_deriv
    *             Second time derivative of the input quaternion.
    */
   static void second_derivative (
      const double * ER7_UTILS_RESTRICT quaternion,
      const double * ER7_UTILS_RESTRICT angular_vel,
      const double * ER7_UTILS_RESTRICT angular_acc,
      double * ER7_UTILS_RESTRICT quaternion_second_deriv);
};


/**
 * Specialization of GeneralizedPositionStepFunctions for the
 * rotation group SO(3) in which
 *  - Generalized position is represented by a parent to body
 *    left transformation quaternion in which the real scalar part is the
 *    initial (zeroth) element of the quaternion and the imaginary vectorial
 *    part comprises the remaining three elements, and
 *  - Generalized velocity is represented by the angular velocity of
 *    the body frame with respect to the parent frame but expressed
 *    in body frame coordinates.
 */
class LeftQuaternionGeneralizedPositionStepFunctions :
   public GeneralizedPositionStepFunctions {
public:

   /**
    * LeftQuaternionGeneralizedPositionStepFunctions default constructor.
    */
   LeftQuaternionGeneralizedPositionStepFunctions()
   :
      GeneralizedPositionStepFunctions (expmap, dexpinv)
   { }


   /**
    * Propagate the quaternion given a change dtheta=omega*dt in the angular
    * velocity space per the exponential map applied as a left operator:
    * @f[
    *   \mathcal{Q}_{\text{new}} =
    *   \exp(\vec{Delta\theta}) \mathcal{Q}_{\text{old}}
    * @f]
    * where @f$\exp(\vec{Delta\theta})@f$ is defined as
    * @f[
    *   \exp(\vec{Delta\theta}) \equiv
    *   \begin{bmatrix}
    *     \cos(Delta\theta/2) \\
    *     -\sin(Delta\theta/2) \hat{\Delta\theta}
    *   \end{bmatrix}
    * @f]
    *
    * @param[in]  quaternion_init
    *             Initial inertial to body-fixed left transformation quaternion.
    * @param[in]  dtheta
    *             Conceptually, product of angular velocity and delta t.
    * @param[out] quaternion_end
    *             Propagated quaternion.
    */
   static void expmap (
      const double * quaternion_init,
      const double * dtheta,
      double * quaternion_end);


   /**
    * Transform the angular velocity from the tangent space at exp(dtheta)
    * to the tangent space at identity via dexpinv.
    *
    * @param[in]  angular_vel_in
    *             Angular velocity at exp(dtheta).
    * @param[in]  dtheta
    *             Conceptually, product of angular velocity and delta t.
    * @param[out] angular_vel_out
    *             Angular velocity at identity.
    */
   static void dexpinv (
      const double * angular_vel_in,
      const double * dtheta,
      double * angular_vel_out);

};


/**
 * Convenience class that merges the left quaternions specializations of
 * GeneralizedPositionDerivativeFunctions and GeneralizedPositionStepFunctions.
 */
class LeftQuaternionGeneralizedPositionFunctions :
   public LeftQuaternionGeneralizedPositionDerivativeFunctions,
   public LeftQuaternionGeneralizedPositionStepFunctions {

public:

   LeftQuaternionGeneralizedPositionFunctions ()
   :
      LeftQuaternionGeneralizedPositionDerivativeFunctions (),
      LeftQuaternionGeneralizedPositionStepFunctions ()
   {}

};


}


#endif
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

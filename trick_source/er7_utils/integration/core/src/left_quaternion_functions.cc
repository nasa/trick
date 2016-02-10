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
 * Defines the four member functions of the left quaternion specializations
 * of the generalized position and generalized step classes.
 */

/*
Purpose: ()
*/


#include <cmath>

#include <cstddef>

#include "../include/left_quaternion_functions.hh"


/**
 * @internal
 *
 * @def ONE_OVER_6
 *    1/6
 * @def ONE_OVER_12
 *    1/12
 * @def ONE_OVER_20
 *    1/20
 * @def ONE_OVER_30
 *    1/30
 * @def ONE_OVER_40
 *    1/40
 * @def ONE_OVER_42
 *    1/42
 * @def ONE_OVER_60
 *    1/60
 */
#define ONE_OVER_6    (1.0 /  6.0)
#define ONE_OVER_12   (1.0 / 12.0)
#define ONE_OVER_20   (1.0 / 20.0)
#define ONE_OVER_30   (1.0 / 30.0)
#define ONE_OVER_40   (1.0 / 40.0)
#define ONE_OVER_42   (1.0 / 42.0)
#define ONE_OVER_60   (1.0 / 60.0)


namespace er7_utils {

/**
 * Truncate a small value to zero.
 * @param[in] val  Value to be trunctated.
 * @return 0 if val is small, val otherwise.
 */
static inline double ER7_UTILS_ALWAYS_INLINE
truncate_scalar (
   double val)
{
   return ((val > -1e-36) && (val < 1e-36)) ? 0.0 : val;
}


/**
 * Truncate elements in a 3-vector that contain small values to zero.
 * @param[in]  in     Vector to be scale
 * @param[out] out    Scaled vector
 */
static inline void ER7_UTILS_ALWAYS_INLINE
truncate_vector (
   const double * ER7_UTILS_RESTRICT in,
   double * ER7_UTILS_RESTRICT out)
{
   out[0] = truncate_scalar (in[0]);
   out[1] = truncate_scalar (in[1]);
   out[2] = truncate_scalar (in[2]);
}


/**
 * Compute the square of the magnitude of a 3-vector.
 * Note: The input vector should not contain extremely small values.
 * @param[in] vec  Vector whose square magnitude is to be computed.
 * @return ||vec||^2
 */
static inline double ER7_UTILS_ALWAYS_INLINE
square_magnitude (
   const double vec[3])
{
   return
      vec[0]*vec[0] + vec[1]*vec[1] + vec[2]*vec[2];
}


/**
 * Compute the inner product between two vectors.
 * Note: The input vectors should not contain extremely small values.
 * @param[in] vec1  First vector
 * @param[in] vec2  Second vector
 * @return vec1 . vec2
 */
static inline double ER7_UTILS_ALWAYS_INLINE
inner_product (
   const double vec1[3],
   const double vec2[3])
{
   return
      vec1[0]*vec2[0] + vec1[1]*vec2[1] + vec1[2]*vec2[2];
}


/**
 * Compute the Nth component of the cross product of two vectors.
 * Note: The input vectors should not contain extremely small values.
 * @tparam N  Component index (0, 1, or 2)
 * @param[in] vec1  First vector
 * @param[in] vec2  Second vector
 * @return (vec1 x vec2)[N]
 */
template<int N>
static inline double ER7_UTILS_ALWAYS_INLINE
cross_product_element (
   const double * vec1,
   const double * vec2)
{
   return vec1[(N+1)%3]*vec2[(N+2)%3] - vec1[(N+2)%3]*vec2[(N+1)%3];
}


/**
 * Scale a 3-vector by some constant.
 * @param[in]  in     Vector to be scale
 * @param[in]  scale  Scale factor
 * @param[out] out    Scaled vector
 */
static inline void ER7_UTILS_ALWAYS_INLINE
scale_vector (
   const double * ER7_UTILS_RESTRICT in,
   const double scale,
   double * ER7_UTILS_RESTRICT out)
{
   out[0] = scale * in[0];
   out[1] = scale * in[1];
   out[2] = scale * in[2];
}


/**
 * Scale a 3-vector by some constant, protecting for off-scale low values.
 * @param[in]  in     Vector to be scale
 * @param[in]  scale  Scale factor
 * @param[out] out    Scaled vector
 */
static inline void ER7_UTILS_ALWAYS_INLINE
scale_and_truncate_vector (
   const double * ER7_UTILS_RESTRICT in,
   const double scale,
   double * ER7_UTILS_RESTRICT out)
{
   out[0] = scale * truncate_scalar (in[0]);
   out[1] = scale * truncate_scalar (in[1]);
   out[2] = scale * truncate_scalar (in[2]);
}

}


namespace er7_utils {

// Compute the time derivative of an inertial to body left transformation
// quaternion as a function of the quaternion and angular velocity.
void
LeftQuaternionGeneralizedPositionDerivativeFunctions::derivative (
   const double * ER7_UTILS_RESTRICT quat,
   const double * ER7_UTILS_RESTRICT ang_vel,
   double * ER7_UTILS_RESTRICT qdot)
{
   double qs = truncate_scalar (quat[0]);
   double qv[3];      // Imaginary part of quaternion, truncated
   double mhvel[3];   // -0.5*ang_vel, truncated

   truncate_vector (quat+1, qv);
   scale_and_truncate_vector (ang_vel, -0.5, mhvel);

   // Compute qdot = { 0, -0.5*ang_vel } * q
   qdot[0] = -inner_product(mhvel, qv);
   qdot[1] = qs*mhvel[0] + cross_product_element<0>(mhvel, qv);
   qdot[2] = qs*mhvel[1] + cross_product_element<1>(mhvel, qv);
   qdot[3] = qs*mhvel[2] + cross_product_element<2>(mhvel, qv);
}


// Compute the second time derivative of an inertial to body left
// transformation quaternion as a function of the quaternion, angular
// velocity, and angular acceleration.
void
LeftQuaternionGeneralizedPositionDerivativeFunctions::second_derivative (
   const double * ER7_UTILS_RESTRICT quat,
   const double * ER7_UTILS_RESTRICT ang_vel,
   const double * ER7_UTILS_RESTRICT ang_acc,
   double * ER7_UTILS_RESTRICT qddot)
{
   double qs = truncate_scalar (quat[0]);
   double qv[3];      // Imaginary part of quaternion, truncated
   double omega[3];   // angular velocity, truncated
   double mhacc[3];   // -0.5*ang_acc, truncated

   truncate_vector (quat+1, qv);
   truncate_vector (ang_vel, omega);
   scale_and_truncate_vector (ang_acc, -0.5, mhacc);

   double mhwsq = -0.25 * square_magnitude(omega);

   qddot[0] = mhwsq*qs - inner_product(mhacc, qv);
   qddot[1] = mhwsq*qv[0] + qs*mhacc[0] + cross_product_element<0>(mhacc, qv);
   qddot[2] = mhwsq*qv[1] + qs*mhacc[1] + cross_product_element<1>(mhacc, qv);
   qddot[3] = mhwsq*qv[2] + qs*mhacc[2] + cross_product_element<2>(mhacc, qv);
}


// Propagate the quaternion given a change dtheta=omega*dt in the angular
// velocity space per the exponential map applied as a left operator.
void
LeftQuaternionGeneralizedPositionStepFunctions::expmap (
   double const quat_init[4],
   double const dtheta[3],
   double quat_end[4])
{
   double hdtheta[3];   // 0.5*dtheta, truncated

   scale_and_truncate_vector (dtheta, 0.5, hdtheta);

   // Compute (0.5 * ||dtheta||)^2
   double h_dtheta_sq = square_magnitude (hdtheta);

   // Too small a value means no change in position.
   if (h_dtheta_sq < 1e-34) {
      quat_end[0] = quat_init[0];
      quat_end[1] = quat_init[1];
      quat_end[2] = quat_init[2];
      quat_end[3] = quat_init[3];
      return;
   }

   double h_dtheta_mag = std::sqrt (h_dtheta_sq);
   double qs_init = truncate_scalar (quat_init[0]);
   double qv_init[3];
   double mtheta_hat[3];
   double quat_norm[4];

   truncate_vector (quat_init+1, qv_init);


   // Compute the unit quaternion in the direction of the
   // quaternion time derivative.
   scale_vector (hdtheta, -1.0/h_dtheta_mag, mtheta_hat);

   quat_norm[0] = -inner_product (mtheta_hat, qv_init);
   quat_norm[1] = qs_init*mtheta_hat[0] +
                  cross_product_element<0>(mtheta_hat, qv_init);
   quat_norm[2] = qs_init*mtheta_hat[1] +
                  cross_product_element<1>(mtheta_hat, qv_init);
   quat_norm[3] = qs_init*mtheta_hat[2] +
                  cross_product_element<2>(mtheta_hat, qv_init);

   // Use polynomial approximations of 1-cos(h_dtheta) and sin(h_dtheta),
   // valid for angles up to about 2.35 degrees.
   if (h_dtheta_sq < 0.001685) {
      // Approximate 1-cos(0.5*dtheta)
      double omcos_hwdt = 0.5 * h_dtheta_sq *
                          (1.0 - ONE_OVER_12*h_dtheta_sq *
                                 (1.0 - ONE_OVER_30*h_dtheta_sq));
      // Approximate sin(0.5*dtheta)
      double sin_hwdt = h_dtheta_mag *
                        (1.0 - ONE_OVER_6*h_dtheta_sq *
                               (1.0 - ONE_OVER_20*h_dtheta_sq *
                                      (1.0 - ONE_OVER_42*h_dtheta_sq)));

      for (int ii = 0; ii < 4; ++ii) {
         double dq = sin_hwdt*quat_norm[ii] - omcos_hwdt*quat_init[ii];
         quat_end[ii] = quat_init[ii] + dq;
      }
   }

   // Use the exact formulation for large angles.
   else {
      double cos_hwdt = std::cos (h_dtheta_mag);
      double sin_hwdt = std::sin (h_dtheta_mag);
      for (int ii = 0; ii < 4; ++ii) {
         quat_end[ii] = cos_hwdt*quat_init[ii] + sin_hwdt*quat_norm[ii];
      }
   }
}


// Transform the angular velocity from the tangent space at exp(dtheta)
// to the tangent space at identity via dexpinv.
void
LeftQuaternionGeneralizedPositionStepFunctions::dexpinv (
   double const angular_vel_in[3],
   double const dtheta[3],
   double angular_vel_out[3])
{
   double vin[3];
   double dth[3];

   truncate_vector (angular_vel_in, vin);
   truncate_vector (dtheta, dth);

   // Compute (||dtheta||)^2
   double dtheta_sq = square_magnitude(dth);

   // Estimate (1-((||dtheta||/2)/2)*cot((||dtheta||/2)/2))/(||dtheta||)^2
   double fact = ONE_OVER_12 *
                 (1.0 + dtheta_sq*ONE_OVER_60 *
                        (1.0 + dtheta_sq*ONE_OVER_42 *
                               (1.0 + dtheta_sq*ONE_OVER_40)));

   double bracket[3];
   double double_bracket[3];

   bracket[0] = cross_product_element<0>(dth, vin);
   bracket[1] = cross_product_element<1>(dth, vin);
   bracket[2] = cross_product_element<2>(dth, vin);

   double_bracket[0] = cross_product_element<0>(dth, bracket);
   double_bracket[1] = cross_product_element<1>(dth, bracket);
   double_bracket[2] = cross_product_element<2>(dth, bracket);

   angular_vel_out[0] = vin[0] + 0.5*bracket[0] + fact*double_bracket[0];
   angular_vel_out[1] = vin[1] + 0.5*bracket[1] + fact*double_bracket[1];
   angular_vel_out[2] = vin[2] + 0.5*bracket[2] + fact*double_bracket[2];
}

}
/**
 * @if Er7UtilsUseGroups
 * @}
 * @}
 * @endif
 */

#ifndef RODRIGUES_ROTATION_H
#define RODRIGUES_ROTATION_H

#ifdef __cplusplus
extern "C" {
#endif

/**
* @ingroup TRICK_MATH
* @brief Generate a transformation matrix for rotation about a given line by a given
* angle, using Rodrigues’ formula.
*
* @param C_out - Transformation matrix for final to initial state.
* @param k - Vector in the direction of the rotation Axis.
* @param theta - Angle of rotation in radians.
*/
void RotAboutLineByAngle(double C_out[3][3], double k[3], double theta);

/**
* @ingroup TRICK_MATH
* @brief Generate a transformation matrix to rotate a vector to new a new orientation.
*
* @param v - Original vector.
* @param w - Vector after rotation.
* @param R_out - Rotation matrix, such that w = Rv.
*/
void RotVectorToNewOrientation(double R_out[3][3], double v[3], double w[3]);

#ifdef __cplusplus
}
#endif
#endif

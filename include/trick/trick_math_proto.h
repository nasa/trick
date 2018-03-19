
#ifndef TRICK_MATH_PROTO_H
#define TRICK_MATH_PROTO_H

/*
 * Function prototypes for all functions in ${TRICK_HOME}/sim_services/math
 */

#include "trick/wave_form.h"
#include "trick/reference_frame.h"
#include "trick/rand_generator.h"
#include "trick/RodriguesRotation.h"

#ifdef __cplusplus
extern "C" {
#endif

/** @defgroup TRICK_MATH Trick Math Utilities
    $TRICK_HOME/trick_source/trick_utils/math/include/trick_math.h
    $TRICK_HOME/trick_source/trick_utils/math/include/trick_math_proto.h
 */

/**
* @ingroup TRICK_MATH
* Invert matrix using LU Decomposition method.
*/
int LUD_inv(double **y, double **a, int n, int *indx, double *ycol);

/**
* @ingroup TRICK_MATH
* Invert lower triangular matrix.
*/
int LUT_inv(double **y, double **a, int n);

/**
* @ingroup TRICK_MATH
*
*/
void LU_bksb(double *b, double **a, int n, int *indx);

/**
 * @ingroup TRICK_MATH
 */
 int LU_dcmp(double **a, double d, int n, int *indx, double *vv);

/**
 * @ingroup TRICK_MATH
 * @brief Solve a linear set of equations.
 * Solve a linear set of equations: [A]x = b ; by first decomposing the
 * SYMMETRIC, POSITIVE DEFINITE [A] matrix into a Lower Triangular, [L],
 * form such that:
 *
 *                [A] = [L][L_transpose] .
 *
 *         The then backsubstitute to solve for x:
 *
 *                [L]y = b ; [L_transpose]x = y .
 *
 * Assumptions and Limitations:
 *        (([A] is a square matrix)
 *         ([A] is symmetric)
 *         ([A] is positive definite)
 *         (Only the lower triangular of [A] needs to be loaded.))
 *
 */
int dLU_Choleski(double **A, double **L, double *y, int n, double *b,
                 double *x, int mode);

/**
 * @ingroup TRICK_MATH
 */
int dLU_solver(double **A, double **L, double *y, int n, double *b,
               double *x, int mode);

/**
 * @ingroup TRICK_MATH
 */
double dS_function(double x, double zero_point, double mid_point,
                   double one_point, double sign);

/**
 * @ingroup TRICK_MATH
 */
int euler123(double angle[3], double mat[3][3], int method,
             double *prev, const char *file, int lineno);

/**
 * @ingroup TRICK_MATH
 */
int euler132(double angle[3], double mat[3][3], int method,
             double *prev, const char *file, int lineno);

/**
 * @ingroup TRICK_MATH
 */
int euler213(double angle[3], double mat[3][3], int method,
             double *prev, const char *file, int lineno);

/**
 * @ingroup TRICK_MATH
 */
int euler231(double angle[3], double mat[3][3], int method,
             double *prev, const char *file, int lineno);

/**
 * @ingroup TRICK_MATH
 */
int euler312(double angle[3], double mat[3][3], int method,
             double *prev, const char *file, int lineno);

/**
 * @ingroup TRICK_MATH
 */
int euler321(double angle[3], double mat[3][3], int method,
             double *prev, const char *file, int lineno);

/**
 * @ingroup TRICK_MATH
 */
int euler123_quat(double angle[3], double quat[4], int method,
                  double *prev);

/**
 * @ingroup TRICK_MATH
 */
int euler132_quat(double angle[3], double quat[4], int method,
                  double *prev);

/**
 * @ingroup TRICK_MATH
 */
int euler213_quat(double angle[3], double quat[4], int method,
                  double *prev);

/**
 * @ingroup TRICK_MATH
 */
int euler231_quat(double angle[3], double quat[4], int method,
                  double *prev);

/**
 * @ingroup TRICK_MATH
 */
int euler312_quat(double angle[3], double quat[4], int method,
                  double *prev);

/**
 * @ingroup TRICK_MATH
 */
int euler321_quat(double angle[3], double quat[4], int method,
                  double *prev);

/**
 * @ingroup TRICK_MATH
 * Add matrix mat1 to matrix mat2, assigning the result to sum.
 */
void dm_add(double sum[3][3], double mat1[3][3], double mat2[3][3]);

/**
 * @ingroup TRICK_MATH
 * Copy matrix mat to copy.
 */
void dm_copy(double copy[3][3], double mat[3][3]);

/**
 * @ingroup TRICK_MATH
 * Initialize matrix mat to the identity matrix.
 */
void dm_ident(double mat[3][3]);

/**
 * @ingroup TRICK_MATH
 * Matrix initialization. Initialize every element to zero.
 */
void dm_init(double mat[3][3]);

/**
 * @ingroup TRICK_MATH
 * Invert a 3X3 matrix.
 */
int dm_invert(double inv[3][3], double m[3][3]);

/**
 * @ingroup TRICK_MATH
 * Invert a symmetric 3X3 matrix.
 */
int dm_invert_symm(double inv[3][3], double m[3][3]);

/**
 * @ingroup TRICK_MATH
 * To orthonormalize a 3X3 transformation matrix.
 */
void dm_orthonormal(double m[3][3], int *axis);

/**
 * @ingroup TRICK_MATH
 * Matric print.
 */
void dm_print(double mat[3][3]);

/**
 * @ingroup TRICK_MATH
 * Matrix Scale.
 */
void dm_scale(double prod[3][3], double mat[3][3], double scalar);

/**
 * @ingroup TRICK_MATH
 * Subtract mat2 from mat2 and put the result in diff.
 */
void dm_sub(double diff[3][3], double mat1[3][3], double mat2[3][3]);

/**
 * @ingroup TRICK_MATH
 * Matrix transpose.
 */
void dm_trans(double trans[3][3], double mat[3][3]);

/**
 * @ingroup TRICK_MATH
 * Matrix transpose times matrix.
 */
void dmtxm(double prod[3][3], double mat1[3][3], double mat2[3][3]);

/**
 * @ingroup TRICK_MATH
 * Matrix transpose times matrix transpose.
 */
void dmtxmt(double prod[3][3], double mat1[3][3], double mat2[3][3]);

/**
 * @ingroup TRICK_MATH
 * Matrix transpose times vector.
 */
void dmtxv(double prod[], double mat[3][3], double vect[]);

/**
 * @ingroup TRICK_MATH
 * Matrix times matrix.
 */
void dmxm(double prod[3][3], double mat1[3][3], double mat2[3][3]);

/**
 * @ingroup TRICK_MATH
 * Matrix times matrix transpose.
 */
void dmxmt(double prod[3][3], double mat1[3][3], double mat2[3][3]);

/**
 * @ingroup TRICK_MATH
 * Matrix times vector.
 */
void dmxv(double prod[], double mat[3][3], double vect[]);

/**
 * @ingroup TRICK_MATH
 *
 */
double drandom_gaussian(double std_deviation, int range);

/**
 * @ingroup TRICK_MATH
 *
 */
void dsingle_axis_rot(int problem, double *phi, double rot_vec[3],
                      double trans_mat[3][3]);

/**
 * @ingroup TRICK_MATH
 * Vector addition.
 */
void dv_add(double sum[], double vect1[], double vect2[]);

/**
 * @ingroup TRICK_MATH
 * Vector copy.
 */
void dv_copy(double copy[], double vect[]);

/**
 * @ingroup TRICK_MATH
 * Vector cross product.
 */
void dv_cross(double cross[], double vect1[], double vect2[]);

/**
 * @ingroup TRICK_MATH
 * Vector dot product.
 */
double dv_dot(double vect1[], double vect2[]);

/**
 * @ingroup TRICK_MATH
 * Vector initialization.
 */
void dv_init(double vect[]);

/**
 * @ingroup TRICK_MATH
 * Vector magnitude.
 */
double dv_mag(double vect[]);

/**
 * @ingroup TRICK_MATH
 * Normalize vector.
 */
void dv_norm(double norm[3], double vect[3]);

/**
 * @ingroup TRICK_MATH
 * Print vector.
 */
void dv_print(double vect[]);

/**
 * @ingroup TRICK_MATH
 * Scale vector.
 */
void dv_scale(double prod[], double vect[], double scalar);

/**
 * @ingroup TRICK_MATH
 * Skew Vector.
 */
void dv_skew(double skew[3][3], double vect[]);

/**
 * @ingroup TRICK_MATH
 * Vector store with a scalar. V=[s,s,s];
 */
void dv_store(double vect[], double scalar);

/**
 * @ingroup TRICK_MATH
 * Vector subtraction. diff = vect1 - vect2;
 */
void dv_sub(double diff[], double vect1[], double vect2[]);

/**
 * @ingroup TRICK_MATH
 * Vector times matrix.
 */
void dvxm(double prod[], double vect[], double mat[3][3]);

/**
 * @ingroup TRICK_MATH
 * sum += vect1 x vect2
 */
void dvxv_add(double sum[], double vect1[], double vect2[]);

/**
 * @ingroup TRICK_MATH
 * sum -= vect1 x vect2
 */
void dvxv_sub(double diff[], double vect1[], double vect2[]);

/**
 * @ingroup TRICK_MATH
 * Householder reduction of symmetric matrix to tri-diagonal form.
 * Assumptions and Limitations:
 *
 * -Input matrix a is symmetric
 * -Input matrix a is destroyed
 */
void eigen_hh_red(double **a, int n, double *d, double *e);

/**
 * @ingroup TRICK_MATH
 * This is a super-duper number munching algorithm that presumably does something useful.
 */
int eigen_jacobi(double **k, double **mass, double **v, double *alpha,
                 int m, int mmax, int sort);

/**
 * @ingroup TRICK_MATH
 * This is a super-duper number munching algorithm that presumably does something useful.
 */
void eigen_jacobi_4(double k[4][4], double mass[4][4],
                    double v[4][4], double alpha[4], int m,
                    int mmax, int sort);

/**
 * @ingroup TRICK_MATH
 * Eigenvalues and eigenvectors of symmetric matrix.
 */
void eigen_ql(double *d, double *e, int n, double **z);

/**
 * @ingroup TRICK_MATH
 * Generate a transformation matrix using a mutually exculsive Euler
 * angle sequence OR generate a mutually exclusive Euler angle sequence
 * using a coordinate transformation matrix.
 */
int euler_matrix(double angle[3], double mat[3][3], int method,
                 Euler_Seq sequence);

/**
 * @ingroup TRICK_MATH
 * Generate a quaternion matix using a mutually exculsive Euler
 * angle sequence OR generate a mutually exclusive Euler angle sequence
 * using a quaternion matrix.
 */
int euler_quat(double angle[3], double quat[4], int method,
                 Euler_Seq sequence);

/**
 * @ingroup TRICK_MATH
 * Matrix copy
 */
void mat_copy(double **copy, double **mat, int m, int n);

/**
 * @ingroup TRICK_MATH
 */
void mat_permute(double **A_coeff, double *B_coeff, double **A_star,
                 double *B_star, int *new_dof, int num_states);

/**
 * @ingroup TRICK_MATH
 */
void mat_print(double **mat, int n);

/**
 * @ingroup TRICK_MATH
 * Compute the normalized quaternion from the corresponding orthonormal transformation matrix.
 */
void mat_to_quat(double quat[4], double a[3][3]);

/**
 * @ingroup TRICK_MATH
 * Matrix transpose.
 */
void mat_trans(double **trans, double **mat, int n);

/**
 * @ingroup TRICK_MATH
 * Matrix multiply.
 */
void matxmat(double **prod, double **mat1, double **mat2, int n);

/**
 * @ingroup TRICK_MATH
 * Matrix times matrix transpose.
 */
void matxtrans(double **prod, double **mat1, double **mat2, int n);

/**
 * @ingroup TRICK_MATH
 * Matrix times vector.
 */
void matxvec(double *prod, double **mat, double *vec, int n);

/**
 * @ingroup TRICK_MATH
 * Multiply two quaternians.
 */
void quat_mult(double q3[4], double q1[4], double q2[4]);

/**
 * @ingroup TRICK_MATH
 */
void quat_norm(double q2[4], double q1[4]);

/**
 * @ingroup TRICK_MATH
 */
void quat_norm_integ(double q2[4], double q1[4]);

/**
 * @ingroup TRICK_MATH
 */
void quat_to_mat(double a[3][3], double quat[4]);

/**
 * @ingroup TRICK_MATH
 */
double roundoff(double res, double val);

/**
 * @ingroup TRICK_MATH
 */
void transxmat(double **prod, double **mat1, double **mat2, int n);

/**
 * @ingroup TRICK_MATH
 */
void transxtrans(double **prod, double **mat1, double **mat2, int n);

/**
 * @ingroup TRICK_MATH
 */
void transxvec(double *prod, double **mat, double *vec, int n);

/**
 * @ingroup TRICK_MATH
 */
void trns_fnct_1o(int num_vars, double in_0[], double in_t[], double dt,
                  double tau[], double out_0[], double out_t[],
                  int *init);

/**
 * @ingroup TRICK_MATH
 */
void trns_fnct_2o(double *z, double *wn, double dt, double *in,
                  double *inwk, double *out, double *outwk, int nd,
                  int *init);

/**
 * @ingroup TRICK_MATH
 */
void vec_print(double *vec, int n);

/**
 * @ingroup TRICK_MATH
 */
double wave_form(WAVE_FORM * W, double my_time);

/**
 * @ingroup TRICK_MATH
 */
double rand_num(RAND_GENERATOR * G);

/**
 * @ingroup TRICK_MATH
 */
double uniform_rnd_1(RAND_GENERATOR * G);

/**
 * @ingroup TRICK_MATH
 */
double uniform_rnd_triple(RAND_GENERATOR * G);

/**
 * @ingroup TRICK_MATH
 */
double gauss_rnd_pseudo(RAND_GENERATOR * G);

/**
* @ingroup TRICK_MATH
*/
double gauss_rnd_bell(RAND_GENERATOR * G);

#ifdef __cplusplus
}
#endif

/*
 * Function macros to provide backward compatability with earlier versions
 */
#define deuler_123( ang, mat, method ) \
          euler123( ang, mat, method, (double*)0, __FILE__, __LINE__ )
#define deuler_132( ang, mat, method ) \
          euler132( ang, mat, method, (double*)0, __FILE__, __LINE__ )
#define deuler_213( ang, mat, method ) \
          euler213( ang, mat, method, (double*)0, __FILE__, __LINE__ )
#define deuler_231( ang, mat, method ) \
          euler231( ang, mat, method, (double*)0, __FILE__, __LINE__ )
#define deuler_312( ang, mat, method ) \
          euler312( ang, mat, method, (double*)0, __FILE__, __LINE__ )
#define deuler_321( ang, mat, method ) \
          euler321( ang, mat, method, (double*)0, __FILE__, __LINE__ )
#define euler_123( ang, mat, method, prev ) \
         euler123( ang, mat, method, prev, __FILE__, __LINE__ )
#define euler_132( ang, mat, method, prev ) \
         euler132( ang, mat, method, prev, __FILE__, __LINE__ )
#define euler_213( ang, mat, method, prev ) \
         euler213( ang, mat, method, prev, __FILE__, __LINE__ )
#define euler_231( ang, mat, method, prev ) \
         euler231( ang, mat, method, prev, __FILE__, __LINE__ )
#define euler_312( ang, mat, method, prev ) \
         euler312( ang, mat, method, prev, __FILE__, __LINE__ )
#define euler_321( ang, mat, method, prev ) \
         euler321( ang, mat, method, prev, __FILE__, __LINE__ )

#define deuler_123_quat( ang, quat, method ) \
          euler123_quat( ang, quat, method, (double*)0 )
#define deuler_132_quat( ang, quat, method ) \
          euler132_quat( ang, quat, method, (double*)0 )
#define deuler_213_quat( ang, quat, method ) \
          euler213_quat( ang, quat, method, (double*)0 )
#define deuler_231_quat( ang, quat, method ) \
          euler231_quat( ang, quat, method, (double*)0 )
#define deuler_312_quat( ang, quat, method ) \
          euler312_quat( ang, quat, method, (double*)0 )
#define deuler_321_quat( ang, quat, method ) \
          euler321_quat( ang, quat, method, (double*)0 )

#endif

/*
PURPOSE:
    (Generate a transformation matrix using an Euler YAW-PITCH-ROLL
     sequence OR generate an Euler YAW-PITCH-ROLL sequence using an
     coordinate transformation matrix.)

PROGRAMMERS:
    (((Robert W. Bailey) (LinCom Corp) (2/1/91))
     ((An Huynh) (LinCom Corp) (June 97) (Singularity Handling))
     ((Robert Gay) (Titan-LinCom) (July 2001) (Added tolerance for
       asin function)))
*/

/* FUNCTION:    A transformation matrix is generated using the input
        YAW-PITCH-ROLL euler angles (in radians). The angles
        are input as:
        angle[0] = YAW      - M_PI   <= YAW   < M_PI
        angle[1] = PITCH    - M_PI/2 <= PITCH < M_PI/2
        angle[2] = ROLL     - M_PI   <= ROLL  < M_PI
*/

#include <stdio.h>
#include "trick/trick_math.h"

int euler321(
        double angle[3],    /* In:  r   method=0, 0=YAW, 1=PITCH, 2=ROLL */
        double mat[3][3],   /* Out: r   Method=0,
                                        Coordinate tranformation matrix   */
        int method,         /* In: 0 = Make matrix from angles,
                                   1 = Make angles from matrix,
                                   2 = Make angles from matrix but use previous
                                       values to prevent singularities */
        double *prev,       /* In: r   prev[3], Previous values of euler
                                       angles */
        const char *file,         /* In: file_name of caller of this function */
        int lineno)         /* In: line # of call to this function in fname */
{

        double s1;              /* SINE OF YAW */
        double c1;              /* COSINE OF YAW */
        double s2;              /* SINE OF PITCH */
        double c2;              /* COSINE OF PITCH */
        double s3;              /* SINE OF ROLL */
        double c3;              /* COSINE OF ROLL */
        double tmp;
        int ret = 0;
        static unsigned short error_flag[5] = {0, 0, 0, 0, 0}; /* Send errors only once */

        (void)file ; /* unused */
        (void)lineno ; /* unused */

        if (method == 0) {

                /* Compute sines and cosines of yaw, pitch, and roll */
                s1 = sin(angle[0]);
                c1 = cos(angle[0]);
                s2 = sin(angle[1]);
                c2 = cos(angle[1]);
                s3 = sin(angle[2]);
                c3 = cos(angle[2]);

                /* Compute values for matrix "mat" */
                mat[0][0] = c2 * c1;
                mat[1][0] = -c3 * s1 + s3 * s2 * c1;
                mat[2][0] = s3 * s1 + c3 * s2 * c1;
                mat[0][1] = c2 * s1;
                mat[1][1] = c3 * c1 + s3 * s2 * s1;
                mat[2][1] = -s3 * c1 + c3 * s2 * s1;
                mat[0][2] = -s2;
                mat[1][2] = s3 * c2;
                mat[2][2] = c3 * c2;
        } else if (method == 1) {
#define TOLERANCE 1.0e-15
                /* Within normal range for asin function */
                if (-1.0 <= -mat[0][2] && -mat[0][2] <= 1.0) {
                        angle[1] = asin(-mat[0][2]);
                        if (M_ABS(angle[1] - M_PI_2) < 1.0e-6) {
                                angle[0] = atan2(mat[2][1], mat[2][0]);
                                angle[1] = M_PI_2;
                                angle[2] = 0.0;
                                ret = TM_SING_321_P;
                                if ( error_flag[0] == 0 ) {
                                    tm_print_error(ret);
                                    error_flag[0]=1;
                                }
                        } else if (M_ABS(angle[1] + M_PI_2) < 1.0e-6) {
                                angle[0] = atan2(-mat[2][1], -mat[2][0]);
                                angle[1] = -M_PI_2;
                                angle[2] = 0.0;
                                ret = TM_SING_321_N;
                                if ( error_flag[1] == 0 ) {
                                    tm_print_error(ret);
                                    error_flag[1]=1;
                                }
                        } else {
                                angle[0] = atan2(mat[0][1], mat[0][0]);
                                angle[2] = atan2(mat[1][2], mat[2][2]);
                        }
                }
                /* Out of normal range for asin func, but within tolerance */
                else if (1.0 < -mat[0][2] && -mat[0][2] <= (1.0 + TOLERANCE)) {
                        angle[0] = atan2(mat[2][1], mat[2][0]);
                        angle[1] = M_PI_2;
                        angle[2] = 0.0;
                        ret = TM_SING_321_P;
                        if ( error_flag[2] == 0 ) {
                            tm_print_error(ret);
                            error_flag[2]=1;
                        }
                } else if ((-1.0 - TOLERANCE) <= -mat[0][2]
                           && -mat[0][2] < -1.0) {
                        angle[0] = atan2(-mat[2][1], -mat[2][0]);
                        angle[1] = -M_PI_2;
                        angle[2] = 0.0;
                        ret = TM_SING_321_N;
                        if ( error_flag[3] == 0 ) {
                            tm_print_error(ret);
                            error_flag[3]=1;
                        }
                }
                /* Error: Out of normal range & beyond tolerance for asin func*/
                else {
                        double zero = 0.0;
                        ret = TM_ANG_NAN;
                        if ( error_flag[4] == 0 ) {
                            tm_print_error(ret);
                            error_flag[4]=1;
                        }
                        angle[0] = angle[1] = angle[2] = 0.0 / zero;
                }
#undef TOLERANCE
        } else if (method == 2) {
#define TOLERANCE 0.0314159265358979    /* 1.8 degree tolerance */

                /* Compute euler angles from tranformation */
                if (M_ABS(mat[0][2] + 1.0) < 1.0e-6) {
                        angle[0] = atan2(mat[2][1], mat[2][0]) + prev[2];
                        angle[1] = M_PI_2;
                        angle[2] = prev[2];
                        tmp = angle[0] - prev[0];
                        if (M_ABS(tmp - 2.0 * M_PI) < TOLERANCE)
                                angle[0] -= 2.0 * M_PI;
                        else if (M_ABS(tmp + 2.0 * M_PI) < TOLERANCE)
                                angle[0] += 2.0 * M_PI;

                } else if (M_ABS(mat[0][2] - 1.0) < 1.0e-6) {
                        angle[0] = atan2(-mat[2][1], -mat[2][0]) + prev[2];
                        angle[1] = -M_PI_2;
                        angle[2] = prev[2];
                        tmp = angle[0] - prev[0];
                        if (M_ABS(tmp - 2.0 * M_PI) < TOLERANCE)
                                angle[0] -= 2.0 * M_PI;
                        else if (M_ABS(tmp + 2.0 * M_PI) < TOLERANCE)
                                angle[0] += 2.0 * M_PI;
                } else {
                        angle[0] = atan2(mat[0][1], mat[0][0]);
                        angle[1] = asin(-mat[0][2]);
                        angle[2] = atan2(mat[1][2], mat[2][2]);
                }

#undef TOLERANCE
        }

        return (ret);
}

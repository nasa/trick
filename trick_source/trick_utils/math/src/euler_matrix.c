/*
   PURPOSE: (Generate a transformation matrix using a mutually exculsive Euler angle sequence OR generate a mutually
   exclusive Euler angle sequence using a coordinate transformation matrix.)

   ASSUMPTIONS AND LIMITATIONS: ((Euler angle sequences must be mutually exclusive, no duplicate axes))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (2/1/91) (Trick-CR-00000) (Initial Release))) */

#include "trick/reference_frame.h"
#include "trick/trick_math.h"

int euler_matrix(double angle[3],       /* In: r Method=0, Euler angles */
                 double mat[3][3],      /* Out: r Method=0, Coordinate transformation matrix */
                 int method,    /* In: 0 = Make matrix from angles, 1 = Make angles from matrix */
                 Euler_Seq sequence)
{                                      /* In: Euler angle sequence for 'angle' */

    int ret = 0;

    switch (sequence) {
        case Roll_Pitch_Yaw:
            ret = deuler_123(angle, mat, method);
            break;
        case Roll_Yaw_Pitch:
            ret = deuler_132(angle, mat, method);
            break;
        case Pitch_Yaw_Roll:
            ret = deuler_231(angle, mat, method);
            break;
        case Pitch_Roll_Yaw:
            ret = deuler_213(angle, mat, method);
            break;
        case Yaw_Roll_Pitch:
            ret = deuler_312(angle, mat, method);
            break;
        case Yaw_Pitch_Roll:
            ret = deuler_321(angle, mat, method);
            break;
        default:
            ret = TM_INV_ROT_SEQ;
    }

    return (ret);

}

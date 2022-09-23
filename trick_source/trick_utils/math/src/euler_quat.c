
#include "trick/reference_frame.h"
#include "trick/trick_math.h"

int euler_quat (double angle[3],        /*   In: r Method=0, Euler angles */
                 double quat[4],         /* Out: r Method=0, quaternion matrix */
                 int method,             /*  In: 0 = Make quaternion from angles, 1 = Make angles from matrix */
                 Euler_Seq sequence)     /*  In: Euler angle sequence for 'angle' */
{
    int ret = 0;

    switch (sequence) {
        case Roll_Pitch_Yaw:
            ret = deuler_123_quat(angle, quat, method);
            break;
        case Roll_Yaw_Pitch:
            ret = deuler_132_quat(angle, quat, method);
            break;
        case Pitch_Yaw_Roll:
            ret = deuler_231_quat(angle, quat, method);
            break;
        case Pitch_Roll_Yaw:
            ret = deuler_213_quat(angle, quat, method);
            break;
        case Yaw_Roll_Pitch:
            ret = deuler_312_quat(angle, quat, method);
            break;
        case Yaw_Pitch_Roll:
            ret = deuler_321_quat(angle, quat, method);
            break;
        default:
            ret = TM_INV_ROT_SEQ;
    }

    return (ret);

}

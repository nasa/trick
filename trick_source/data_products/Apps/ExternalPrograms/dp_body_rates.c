/*
 PURPOSE:
     (Transform Inertial referenced body rates to the body frame.)

 REFERENCE:
     ((None))

 ASSUMPTIONS AND LIMITATIONS:
     ((None))

 PROGRAMMERS:
     (((Robert W. Bailey) (LinCom Corp) (2/14/92) (--) (Rob's Whim))
       (Dorci L. Smith) (TSC-AEU) (May 2002) (Reduce ext program size))
*/

#include <stdio.h>
#include "trick_utils/math/include/matrix_macros.h"

int extGetNextRecord(double *in, int numIn, double *out, int numOut)
{

        double inrtl_body[3][3];

        /* Set initial matrix with input from SIM */
        inrtl_body[0][0] = in[3];
        inrtl_body[1][0] = in[4];
        inrtl_body[2][0] = in[5];
        inrtl_body[0][1] = in[6];
        inrtl_body[1][1] = in[7];
        inrtl_body[2][1] = in[8];
        inrtl_body[0][2] = in[9];
        inrtl_body[1][2] = in[10];
        inrtl_body[2][2] = in[11];

        /* Perform matrix function */
        MxV(out, inrtl_body, in);

        return (0);
}

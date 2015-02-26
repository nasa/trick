/*
PURPOSE:
    (Extract Inertial referenced Euler angles from a body to inertial
        transformation matrix.)

REFERENCE:
    ((None))

ASSUMPTIONS AND LIMITATIONS:
    ((Euler sequence is input interactively by user.))

PROGRAMMERS:
    (((Robert W. Bailey) (LinCom Corp) (4/9/91)
       (Trick-CR-00004) (Gravity / Inertial Frame Models))
       (Dorci L. Smith) (TSC-AEU) (May 2002) (Reduce ext program size))
*/

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "trick_utils/math/include/trick_math.h"

#define ROLL  1
#define PITCH 2
#define YAW   3

int extGetNextRecord(double *in, int numIn, double *out, int numOut)
{

        double inrtl_body[3][3];
        static int status = 0;
        static int a[3];
        FILE *pipe;
        char i[10];
        char j[10];
        char k[10];

        /* Call Tcl GUI to choose angle sequence */
        if (status == 0) {
                if ((pipe =
                     popen("/user3/test_acct/Tcl/choose_sequence.tcl", "r"))
                    != NULL) {
                        fscanf(pipe, "%s %s %s\n", i, j, k);
                }
                status = 1;
                pclose(pipe);

                a[0] = atoi(i);
                a[1] = atoi(j);
                a[2] = atoi(k);
        }

        /* Set initial matrix with input from SIM */
        inrtl_body[0][0] = in[0];
        inrtl_body[1][0] = in[1];
        inrtl_body[2][0] = in[2];
        inrtl_body[0][1] = in[3];
        inrtl_body[1][1] = in[4];
        inrtl_body[2][1] = in[5];
        inrtl_body[0][2] = in[6];
        inrtl_body[1][2] = in[7];
        inrtl_body[2][2] = in[8];

        /* Call external programs according to angle sequence */
        if (a[0] == ROLL && a[1] == PITCH && a[2] == YAW)
                euler123(out, inrtl_body, 1, out, "", 0);
        else if (a[0] == ROLL && a[1] == YAW && a[2] == PITCH)
                euler132(out, inrtl_body, 1, out, "", 0);
        else if (a[0] == PITCH && a[1] == ROLL && a[2] == YAW)
                euler213(out, inrtl_body, 1, out, "", 0);
        else if (a[0] == PITCH && a[1] == YAW && a[2] == ROLL)
                euler231(out, inrtl_body, 1, out, "", 0);
        else if (a[0] == YAW && a[1] == ROLL && a[2] == PITCH)
                euler312(out, inrtl_body, 1, out, "", 0);
        else if (a[0] == YAW && a[1] == PITCH && a[2] == ROLL)
                euler321(out, inrtl_body, 1, out, "", 0);

        return (0);
}

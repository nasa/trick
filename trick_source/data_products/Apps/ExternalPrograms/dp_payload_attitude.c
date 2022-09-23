/*
PURPOSE:
    (Extract Inertial referenced and OBAS referenced Euler angles from
     OBAS to inertial and payload body to inertial transformation matrices.)

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

        double inrtl_payload[3][3];     /* Inertial to payload body transformation matrix */
        double obas_inrtl[3][3];        /* OBAS to Inertial transformation matrix */
        double obas_payload[3][3];      /* OBAS to payload body transformation matrix */
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

        /* Set initial matricies with input from SIM. Run matrix functions. */
        inrtl_payload[0][0] = in[0];
        inrtl_payload[1][0] = in[1];
        inrtl_payload[2][0] = in[2];
        inrtl_payload[0][1] = in[3];
        inrtl_payload[1][1] = in[4];
        inrtl_payload[2][1] = in[5];
        inrtl_payload[0][2] = in[6];
        inrtl_payload[1][2] = in[7];
        inrtl_payload[2][2] = in[8];

        obas_inrtl[0][0] = in[9];
        obas_inrtl[0][1] = in[10];
        obas_inrtl[0][2] = in[11];
        obas_inrtl[1][0] = in[12];
        obas_inrtl[1][1] = in[13];
        obas_inrtl[1][2] = in[14];
        obas_inrtl[2][0] = in[15];
        obas_inrtl[2][1] = in[16];
        obas_inrtl[2][2] = in[17];

        MxM(obas_payload, inrtl_payload, obas_inrtl);

        /* Call external programs according to angle sequence */
        if (a[0] == ROLL && a[1] == PITCH && a[2] == YAW) {
                euler123(out, inrtl_payload, 1, out, "", 0);
                euler123(&(out[3]), obas_payload, 1, out, "", 0);
        } else if (a[0] == ROLL && a[1] == YAW && a[2] == PITCH) {
                euler132(out, inrtl_payload, 1, out, "", 0);
                euler132(&(out[3]), obas_payload, 1, out, "", 0);
        } else if (a[0] == PITCH && a[1] == ROLL && a[2] == YAW) {
                euler213(out, inrtl_payload, 1, out, "", 0);
                euler213(&(out[3]), obas_payload, 1, out, "", 0);
        } else if (a[0] == PITCH && a[1] == YAW && a[2] == ROLL) {
                euler231(out, inrtl_payload, 1, out, "", 0);
                euler231(&(out[3]), obas_payload, 1, out, "", 0);
        } else if (a[0] == YAW && a[1] == ROLL && a[2] == PITCH) {
                euler312(out, inrtl_payload, 1, out, "", 0);
                euler312(&(out[3]), obas_payload, 1, out, "", 0);
        } else if (a[0] == YAW && a[1] == PITCH && a[2] == ROLL) {
                euler321(out, inrtl_payload, 1, out, "", 0);
                euler321(&(out[3]), obas_payload, 1, out, "", 0);
        }

        return (0);
}

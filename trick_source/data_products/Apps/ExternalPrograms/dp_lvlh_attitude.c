/*
PURPOSE:
      (Extract LVLH referenced Euler angles from a body to inertial
              transformation matrix.)

REFERENCE:
      ((None))

ASSUMPTIONS AND LIMITATIONS:
      ((Euler sequence is input interactively by user.))

PROGRAMMERS:
      (((John J. Whynott) (McDonnell Douglas Corp) (4/30/91)
        (Trick-CR-00004) (Gravity / Inertial Frame Models))
        (Dorci L. Smith) (TSC-AEU) (May 2002) (Reduce ext program size))
*/

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

void mat_lvlh_inertial(double lvlh_inrtl[3][3],
                       double inrtl_pos[3], double inrtl_vel[3]);
#include "trick_utils/math/include/trick_math.h"

#define ROLL  1
#define PITCH 2
#define YAW   3

int extGetNextRecord(double *in, int numIn, double *out, int numOut)
{

        double inrtl_pos[3];
        double inrtl_vel[3];
        double inrtl_body[3][3];
        double lvlh_inrtl[3][3];
        double lvlh_body[3][3];
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
        inrtl_pos[0] = in[0];
        inrtl_pos[1] = in[1];
        inrtl_pos[2] = in[2];
        inrtl_vel[0] = in[3];
        inrtl_vel[1] = in[4];
        inrtl_vel[2] = in[5];
        inrtl_body[0][0] = in[6];
        inrtl_body[1][0] = in[7];
        inrtl_body[2][0] = in[8];
        inrtl_body[0][1] = in[9];
        inrtl_body[1][1] = in[10];
        inrtl_body[2][1] = in[11];
        inrtl_body[0][2] = in[12];
        inrtl_body[1][2] = in[13];
        inrtl_body[2][2] = in[14];

        mat_lvlh_inertial(lvlh_inrtl, inrtl_pos, inrtl_vel);

        MxM(lvlh_body, inrtl_body, lvlh_inrtl);

        /*Call external programs according to angle sequence */
        if (a[0] == ROLL && a[1] == PITCH && a[2] == YAW)
                euler123(out, lvlh_body, 1, out, "", 0);
        else if (a[0] == ROLL && a[1] == YAW && a[2] == PITCH)
                euler132(out, lvlh_body, 1, out, "", 0);
        else if (a[0] == PITCH && a[1] == ROLL && a[2] == YAW)
                euler213(out, lvlh_body, 1, out, "", 0);
        else if (a[0] == PITCH && a[1] == YAW && a[2] == ROLL)
                euler231(out, lvlh_body, 1, out, "", 0);
        else if (a[0] == YAW && a[1] == ROLL && a[2] == PITCH)
                euler312(out, lvlh_body, 1, out, "", 0);
        else if (a[0] == YAW && a[1] == PITCH && a[2] == ROLL)
                euler321(out, lvlh_body, 1, out, "", 0);

        return (0);
}

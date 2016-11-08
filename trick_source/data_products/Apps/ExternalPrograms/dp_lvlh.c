/*
PURPOSE:
       (Compute LVLH relative position and velocity given
        two inertial state vectors)

REFERENCE:
       ((None))

ASSUMPTIONS AND LIMITATIONS:
       ()

PROGRAMMERS:
       (((Robert W. Bailey) (LinCom) (11/91) (--) (SAFER Model))
        (Dorci L. Smith) (TSC-AEU) (May 2002) (Reduce ext program size))
*/

#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>

#include "trick_utils/math/include/matrix_macros.h"
#include "trick_utils/math/include/vector_macros.h"

void mat_lvlh_inertial(double lvlh_inrtl[3][3], double pos[3], double vel[3]);

int extGetNextRecord(double *in, int numIn, double *out, int numOut)
{

        double lvlh_inrtl[3][3];        /* LVLH to inertial transformation matrix */
        double inrtl_pos[3];    /* Inertial position of body */
        double inrtl_vel[3];    /* Inertial velocity of body */
        double ref_pos[3];      /* Inertial position of reference body */
        double ref_vel[3];      /* Inertial velocity of reference body */
        double rel_pos[3];      /* relative Inertial position between bodies */
        double rel_vel[3];      /* relative Inertial velocity between bodies */
        double lvlh_pos[3];     /* relative LVLH position between bodies */
        double lvlh_vel[3];     /* relative LVLH velocity between bodies */
        double worb;            /* Orbital rate */

        /* Set initial matricies with input from SIM. Run matrix functions. */
        inrtl_pos[0] = in[0];
        inrtl_pos[1] = in[1];
        inrtl_pos[2] = in[2];
        inrtl_vel[0] = in[3];
        inrtl_vel[1] = in[4];
        inrtl_vel[2] = in[5];
        ref_pos[0] = in[6];
        ref_pos[1] = in[7];
        ref_pos[2] = in[8];
        ref_vel[0] = in[9];
        ref_vel[1] = in[10];
        ref_vel[2] = in[11];

        rel_pos[0] = inrtl_pos[0] - ref_pos[0];
        rel_pos[1] = inrtl_pos[1] - ref_pos[1];
        rel_pos[2] = inrtl_pos[2] - ref_pos[2];
        rel_vel[0] = inrtl_vel[0] - ref_vel[0];
        rel_vel[1] = inrtl_vel[1] - ref_vel[1];
        rel_vel[2] = inrtl_vel[2] - ref_vel[2];

        mat_lvlh_inertial(lvlh_inrtl, ref_pos, ref_vel);

        worb = -V_DOT(ref_vel, lvlh_inrtl[0]) / V_MAG(ref_pos);

        VxM(lvlh_pos, rel_pos, lvlh_inrtl);
        VxM(lvlh_vel, rel_vel, lvlh_inrtl);

        out[0] = lvlh_pos[0];
        out[1] = lvlh_pos[1];
        out[2] = lvlh_pos[2];
        out[3] = lvlh_vel[0];
        out[4] = lvlh_vel[1];
        out[5] = lvlh_vel[2];


        return (0);
}

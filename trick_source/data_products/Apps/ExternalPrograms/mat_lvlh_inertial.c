/*
PURPOSE:
    (Generate a Local Vertical Local Horizontal - LVLH - to inertial
        transformation matrix given an inertial position vector.)
REFERENCE:
    ((None))
ASSUMPTIONS AND LIMITATIONS:
    ((none))
CLASS:
    (N/A)
LIBRARY DEPENDENCY:
    ((mat_lvlh_inertial.o))
PROGRAMMERS:
    (((Robert W. Bailey) (LinCom Corp) (4/2/91)
         (Trick-CR-00004) (Gravity / Inertial Frame Models)))
*/

#include <stdio.h>
#include <float.h>

#include "trick_utils/math/include/vector_macros.h"

#ifndef _ABS
#define _ABS(x) ((x) < 0 ? -(x) : (x))
#endif

    /* ENTRY POINT: */
void mat_lvlh_inertial(
                                  /* RETURN: -- None */
        double lvlh_inrtl[3][3],  /* OUT: --    LVLH to Inertial trans matrix */
        double pos[3],            /* IN: --    Inertial position vector */
        double vel[3])            /* IN: --    Inertial velocity vector */
{

        double lvlhx[3];
        double lvlhy[3];
        double lvlhz[3];
        double temp[3];         /* TEMPORARY WORKSPACE */

        /* ALIGN LVLH-Z WITH NEGATIVE RADIUS VECTOR */
        V_NORM(lvlhz, pos);
        lvlhz[0] = -lvlhz[0];
        lvlhz[1] = -lvlhz[1];
        lvlhz[2] = -lvlhz[2];

        /* CROSS VELOCITY WITH POSITION VEC TO GET LVLH-Y (NEG MOMENTUM VECTOR) */
        V_CROSS(temp, vel, pos);
        V_NORM(lvlhy, temp);

        /* CROSS LVLH-Y WITH LVLH-Z TO GET LVLH-X */
        V_CROSS(temp, lvlhy, lvlhz);
        V_NORM(lvlhx, temp);

        /* LOAD LVLH AXES INTO TRANSFORMATION MATRIX */
        /* LVLH-X IS FIRST COLUMN */
        /* LVLH-Y IS SECOND COLUMN */
        /* LVLH-Z IS THIRD COLUMN */
        lvlh_inrtl[0][0] = lvlhx[0];
        lvlh_inrtl[1][0] = lvlhx[1];
        lvlh_inrtl[2][0] = lvlhx[2];
        lvlh_inrtl[0][1] = lvlhy[0];
        lvlh_inrtl[1][1] = lvlhy[1];
        lvlh_inrtl[2][1] = lvlhy[2];
        lvlh_inrtl[0][2] = lvlhz[0];
        lvlh_inrtl[1][2] = lvlhz[1];
        lvlh_inrtl[2][2] = lvlhz[2];

        return;
}

/*
   PURPOSE: (To orthanormalize a 3X3 transformation matrix.)

   ASSUMPTIONS AND LIMITATIONS: ((One axis of transformation matrix is forfieted.) (This axis rotates between first,
   second, and third axes.))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (4/1/91) (Trick-CR-00004) (Gravity / Inertial Frame Models))) */

#include "trick/trick_math.h"

void dm_orthonormal(            /* Return: -- none */
                       double m[3][3],  /* Inout: Matrix to be orthonormalized. */
                       int *axis)
{                                      /* Inout: Axis to forfiet during orthonormalization */

    double vec[3];              /* Scratch space for taking unit vectors */

    switch (*axis) {
        case 0:
            /* Unitize the second and third rows */
            /* Cross the second and third rows to get the first */
            V_NORM(vec, m[1]);
            V_COPY(m[1], vec);
            V_NORM(vec, m[2]);
            V_COPY(m[2], vec);
            V_CROSS(m[0], m[1], m[2]);
            (*axis)++;
            break;

        case 1:
            /* Unitize the third and first rows */
            /* Cross the third and first rows to get the second */
            V_NORM(vec, m[2]);
            V_COPY(m[2], vec);
            V_NORM(vec, m[0]);
            V_COPY(m[0], vec);
            V_CROSS(m[1], m[2], m[0]);
            (*axis)++;
            break;

        case 2:
            /* Unitize the first and second rows */
            /* Cross the first and second rows to get the third */
            V_NORM(vec, m[0]);
            V_COPY(m[0], vec);
            V_NORM(vec, m[1]);
            V_COPY(m[1], vec);
            V_CROSS(m[2], m[0], m[1]);
            *axis = 0;
            break;
    }

    return;
}

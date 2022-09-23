/*
   PURPOSE: (Convert a transformation matrix to a single axis rotation about a unit vector OR visa versa.)

   ASSUMPTIONS AND LIMITATIONS: ((Orthonormal, right-handed coordinate transformations))

   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (3/22/91) (Trick-CR-00000) (Initial release)) ((David Hammen/Robert
   Gay) (Titan-LinCom) (June/July 2001) (Rewrote backwards problem))) */
/* FUNCTION: 1) Work problem forwards or backwards depending on problem flag. 2) If forwards, work the problem in the
   following manner:

   [GAMMA] = CR*[I] + (1 - CR)*U*UT - SR*[UA] where: GAMMA is the transformation matrix: SYSTEM 1 => SYSTEM 2. CR is
   the cosine of the single axis rotation, SR is the sine " " " " " , I is the identity matrix, U is the unit vector to
   ratate about, UT is the transpose of U, and UA is the alternate (skew) symmetric matrix of U.

   3) Working the problem backwards is based on the following: 1) trace([GAMMA]) = 2*CR + 1 2) SR*UA =
   -([GAMMA]-transpose([GAMMA]))/2 3) ([GAMMA][ii][jj]+[GAMMA][jj][ii])/2 = CR*delta_ij + (1-CR)*UiUj where: U# is a
   single component of the unit vector about which the coordinate frame is rotated.

   The solution of the reverse problem depends on the phi angle: a) The rotation axis is ill-defined for a rotation of
   zero degrees. The algorithm arbitrarily sets the axis to the xhat axis. This condition exists when sinphi is
   identically zero and cosphi is positive. b) Relation (2) most accurately defines the rotation angle and axis for
   non-zero rotation angles less than 45 degrees. This condition exists when sinphi is non-zero and less than cosphi.
   c) Relation (1) most accurately defines the rotation angle but relation (2) best defines the rotation axis for
   angles between 45 and 135 degrees. This condition exists when sinphi exceeds the absolute value of cosphi. d)
   Relation (2) most accurately defines the rotation angle but relation (3) best defines the rotation axis when the
   rotation angle exceeds 135 degrees. This condition exists when cosphi is negative and larger in magnitude than
   sinphi. */

#include <math.h>
#include <stdio.h>

#include "trick/trick_math.h"


void dsingle_axis_rot(          /* Return: -- None */
                         int problem,   /* In: non-zero = Forward solution, 0 = Backward solution */
                         double *phi,   /* Inout: r Rotation eigenangle */
                         double rot_vec[3],     /* Inout: Rotation eigenaxis */
                         double trans_mat[3][3])
{                                      /* Inout: Transformation matrix to the rotated frame */
    double cosphi;              /* cosine of phi */
    double sinphi;              /* sine of phi */
    double rot_unit[3];         /* Unit vector of rot_vec */
    double outer[3][3];         /* Outer product of rot_unit */
    double skew[3][3];          /* Skew-symmetric matrix from rot_unit */
    double coef;                /* 1.0 - cosphi */
    double diff[3];             /* Non-normalized rotation eigen axis */
    double dmag;                /* Magnitude of diff */
    double vscale;              /* Scale factor used to generate rot_unit */
    int ii, jj;


    /* Forward solution: Solve for the transformation matrix given the eigen angle and eigen axis. */
    if (problem) {
        cosphi = cos(*phi);
        sinphi = sin(*phi);
        coef = 1.0 - cosphi;

        V_NORM(rot_unit, rot_vec);
        V_OUTER(outer, rot_unit);
        V_SKEW(skew, rot_unit);

        /* Calculate trans_mat */
        trans_mat[0][0] = cosphi + coef * outer[0][0];
        trans_mat[1][0] = coef * outer[1][0] - sinphi * skew[1][0];
        trans_mat[2][0] = coef * outer[2][0] - sinphi * skew[2][0];
        trans_mat[0][1] = coef * outer[0][1] - sinphi * skew[0][1];
        trans_mat[1][1] = cosphi + coef * outer[1][1];
        trans_mat[2][1] = coef * outer[2][1] - sinphi * skew[2][1];
        trans_mat[0][2] = coef * outer[0][2] - sinphi * skew[0][2];
        trans_mat[1][2] = coef * outer[1][2] - sinphi * skew[1][2];
        trans_mat[2][2] = cosphi + coef * outer[2][2];

        /* Reverse solution: Solve for the eigen angle, axis given the transformation matrix. */
    } else {

        /* Calculate vector pointing along the coordinated turn axis */
        diff[0] = trans_mat[1][2] - trans_mat[2][1];
        diff[1] = trans_mat[2][0] - trans_mat[0][2];
        diff[2] = trans_mat[0][1] - trans_mat[1][0];
        dmag = V_MAG(diff);

        /* Determine the rotation angle sin and cosine using relations (1) and (2). */
        sinphi = 0.5 * dmag;
        cosphi = 0.5 * (trans_mat[0][0] + trans_mat[1][1] + trans_mat[2][2] - 1.0);

        /* sinphi = 0, positive cosphi => phi is zero. The rotation is ill-defined in this case. Arbitrarily set the
           rotation vector to xhat. */
        if ((dmag == 0.0) && (cosphi > 0.0)) {
            *phi = 0.0;
            rot_vec[0] = 1.0;
            rot_vec[1] = 0.0;
            rot_vec[2] = 0.0;

            /* Small sinphi, negative cosphi => phi is 135 degrees or larger.  The anti-symmetric difference does a
               good job of defining the rotation angle but poor job of defining the rotation axis. */
        } else if (sinphi < -cosphi) {

            /* Use relation (2) to determine the rotation angle, taking care to place the angle in the correct
               quadrant. */
            *phi = M_PI - asin(sinphi);

            /* Use relation (3) to determine the rotation axis. */

            /* Ignoring precision loss, any diagonal element of the matrix can be used as the basis for determining
               the rotation axis. The greatest accuracy will result from using the largest diagonal element. */
            jj = 0;
            if (trans_mat[1][1] > trans_mat[jj][jj]) {
                jj = 1;
            }
            if (trans_mat[2][2] > trans_mat[jj][jj]) {
                jj = 2;
            }

            /* Determine the rotation axis element corresponding to the selected diagonal element. */
            coef = 1.0 - cosphi;
            rot_vec[jj] = copysign(sqrt((trans_mat[jj][jj] - cosphi) / coef), diff[jj]);

            /* Determine the remaining elements of the rotation axis vector. */
            vscale = 1. / (2 * coef * rot_vec[jj]);
            for (ii = 0; ii < 3; ii++) {
                if (ii != jj) {
                    rot_vec[ii] = (trans_mat[jj][ii] + trans_mat[ii][jj]) * vscale;
                }
            }

        } else {
            /* Ignoring machine errors, the rotation angle is phi = acos(cosphi) However, acos gives
               less-than-desirable results than does asin for small angles. => Use the inverse trig function best
               suited to the case at hand. */
            if (sinphi < cosphi) {
                *phi = asin(sinphi);
            } else {
                *phi = acos(cosphi);
            }

            /* The diff vector calculated above is directed along the rotation axis. The rotation axis unit vector is
               this diff divided by its magnitude, dmag. */
            vscale = 1. / dmag;
            V_SCALE(rot_vec, diff, vscale);
        }
    }
}

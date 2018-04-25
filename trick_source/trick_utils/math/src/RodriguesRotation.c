#include <math.h>
#include <trick/vector_macros.h>
#include <trick/matrix_macros.h>

void RotAboutLineByAngle(double R_out[3][3], double k[3], double theta) {

   double c = cos(theta);
   double a = 1.0 - c;
   double b = sin(theta);
   double n[3];

   V_NORM(n, k);

   /* Calculate the rotation matrix using the Rodrigues formula. */
   R_out[0][0] = c + 0      + n[0]*n[0]*a;
   R_out[0][1] = 0 - n[2]*b + n[0]*n[1]*a;
   R_out[0][2] = 0 + n[1]*b + n[0]*n[2]*a;

   R_out[1][0] = 0 + n[2]*b + n[0]*n[1]*a;
   R_out[1][1] = c + 0      + n[1]*n[1]*a;
   R_out[1][2] = 0 - n[0]*b + n[1]*n[2]*a;

   R_out[2][0] = 0 - n[1]*b + n[0]*n[2]*a;
   R_out[2][1] = 0 + n[0]*b + n[1]*n[2]*a;
   R_out[2][2] = c + 0      + n[2]*n[2]*a;

}

#define SMALL_NUMBER 1e-10

void RotVectorToNewOrientation(double R_out[3][3], double v[3], double w[3]) {

    double a = V_DOT(v,v);
    double b = V_DOT(v,w);
    double theta = acos(b/a);

    if ( fabs(theta) < SMALL_NUMBER ) {
        M_IDENT(R_out);
    } else {
        double k[3],n[3];
        V_CROSS(k, v, w);
        V_NORM(n, k);
        RotAboutLineByAngle(R_out, n, theta);
    }
}

#include <math.h>

void RotAboutLineByAngle(double C_out[3][3], double k[3], double theta) {

   double c = cos(theta);
   double a = 1.0 - c;
   double b = sin(theta);

   /* Normalize the rotation axis vector */
   double length = sqrt(k[0]*k[0] + k[1]*k[1] + k[2]*k[2]);
   double n[3] = {k[0]/length, k[1]/length, k[2]/length};

   /* Calculate the rotation matrix using the Rodrigues formula. */
   C_out[0][0] = c + 0      + n[0]*n[0]*a;
   C_out[0][1] = 0 - n[2]*b + n[0]*n[1]*a;
   C_out[0][2] = 0 + n[1]*b + n[0]*n[2]*a;

   C_out[1][0] = 0 + n[2]*b + n[0]*n[1]*a;
   C_out[1][1] = c + 0      + n[1]*n[1]*a;
   C_out[1][2] = 0 - n[0]*b + n[1]*n[2]*a;

   C_out[2][0] = 0 - n[1]*b + n[0]*n[2]*a;
   C_out[2][1] = 0 + n[0]*b + n[1]*n[2]*a;
   C_out[2][2] = c + 0      + n[2]*n[2]*a;

}

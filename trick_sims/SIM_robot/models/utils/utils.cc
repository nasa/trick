/********************************* TRICK HEADER *******************************
PURPOSE: ( Linear algebra routines for 2x2 systems)
LIBRARY DEPENDENCY:
    ((utils/utils.o))
ASSUMPTIONS:
    ( Square 2x2 matrices )
PROGRAMMERS:
    (((Sean Harmeyer) (NASA) (Dec 2022) (Trick Example Sim)))
*******************************************************************************/
#include "utils/utils.hh"

void ManipUtils::VCross(double * c, double * a, double * b)
{
    c[0] = a[1] * b[2] - b[1] * a[2];
    c[1] = -(a[0] * b[2] - b[0] * a[2]);
    c[2] = a[0] * b[1] - b[0] * a[1];
}

void ManipUtils::MMMult(double ** c, double ** a, double ** b)
{
    double c11, c12, c21, c22;

    c11 = a[0][0] * b[0][0] + a[0][1] * b[1][0];
    c12 = a[0][0] * b[0][1] + a[0][1] * b[1][1];
    c21 = a[1][0] * b[0][0] + a[1][1] * b[1][0];
    c22 = a[1][0] * b[0][1] + a[1][1] * b[1][1];

    c[0][0] = c11;
    c[0][1] = c12;
    c[1][0] = c21;
    c[1][1] = c22;
}

void ManipUtils::MVMult(double * c, double ** a, double * b)
{
    double c11, c12;

    c11 = a[0][0] * b[0] + a[0][1] * b[1];
    c12 = a[1][0] * b[0] + a[1][1] * b[1];

    c[0] = c11;
    c[1] = c12;
}

void ManipUtils::Transpose(double ** trans, double ** a)
{
    int i, j;
    for(i = 0; i < 2; i++)
    {
        for(j = 0; j < 2; j++)
        {
            trans[i][j] = a[j][i];
        }
    }
}

void ManipUtils::VAdd(double * c, double * a, double * b)
{
    int i;
    for(i = 0; i < 2; i++)
    {
        c[i] = a[i] + b[i];
    }
}

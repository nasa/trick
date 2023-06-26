#ifndef __MANIPUTILS_HH_
#define __MANIPUTILS_HH_
/**************************************************************************
PURPOSE: (2x2 Linear Algebra utilities)
***************************************************************************/



class ManipUtils
{
    public:

        void MMMult(double **c, double **a, double **b);
        void MVMult(double *c, double **a, double *b);
        void Transpose(double **aT, double **a);
        void VCross(double *c, double *a, double *b);
        void VAdd(double *c, double *a, double *b);

        ManipUtils(){};

};

#endif

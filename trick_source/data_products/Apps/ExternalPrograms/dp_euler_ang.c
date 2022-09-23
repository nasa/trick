/*
PURPOSE:
    (Extract Euler angles from transformation matrix.)

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

        static int status = 0;
        static int a[3];        /* Extracted Euler angle sequence */
        FILE *pipe;
        char i[10];
        char j[10];
        char k[10];

        int ii, jj, kk = 0;
        double matrixIn[3][3];
        // double matrixOut[3] ;

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
        // Load output array
        for (ii = 0; ii < 3; ii++) {
                for (jj = 0; jj < 3; jj++) {
                        matrixIn[ii][jj] = in[kk];
                        kk++;
                }
        }


        /* Call external programs according to angle sequence */
        if (a[0] == ROLL && a[1] == PITCH && a[2] == YAW)
                euler123(out, matrixIn, 1, out, "", 0);
        //euler123( matrixOut , matrixIn , 1, out, "", 0 ) ;
        else if (a[0] == ROLL && a[1] == YAW && a[2] == PITCH)
                euler132(out, matrixIn, 1, out, "", 0);
        //euler132( matrixOut , matrixIn , 1, out, "", 0 ) ;
        else if (a[0] == PITCH && a[1] == ROLL && a[2] == YAW)
                euler213(out, matrixIn, 1, out, "", 0);
        //euler213( matrixOut , matrixIn , 1, out, "", 0 ) ;
        else if (a[0] == PITCH && a[1] == YAW && a[2] == ROLL)
                euler231(out, matrixIn, 1, out, "", 0);
        //euler231( matrixOut , matrixIn , 1, out, "", 0 ) ;
        else if (a[0] == YAW && a[1] == ROLL && a[2] == PITCH)
                euler312(out, matrixIn, 1, out, "", 0);
        //euler312( matrixOut , matrixIn , 1, out, "", 0 ) ;
        else if (a[0] == YAW && a[1] == PITCH && a[2] == ROLL)
                euler321(out, matrixIn, 1, out, "", 0);
        //euler321( matrixOut , matrixIn , 1, out, "", 0 ) ;

        // Load output array
        //for ( ii = 0 ; ii < 3 ; ii ) {
        //  out[ii] = matrixOut[ii] ;
        // }


        return (0);
}

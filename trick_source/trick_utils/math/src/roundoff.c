/*
   PURPOSE: (Round off a double precision value to the resolution specified.)
   REFERENCE: ((None))
   ASSUMPTIONS AND LIMITATIONS: ((none))
   CLASS: (N/A)
   LIBRARY DEPENDENCY: ((roundoff.o))
   PROGRAMMERS: (((Robert W. Bailey) (LinCom Corp) (Feb 1991) (v1.0) (Initial Release.)))
 */

#include "trick/trick_math.h"

double roundoff(                /* Return: Value after roundoff */
                   double res,  /* In: Resolution for roundoff function */
                   double val)
{                                      /* In: Value to be truncated */

    double remain;

    remain = fmod(val, res);

    if ((remain / res) <= 0.5)
        return (val - remain);
    else
        return (val + ((1.0 * res) - remain));

}

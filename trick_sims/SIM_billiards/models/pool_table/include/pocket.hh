/********************************* TRICK HEADER *******************************
PURPOSE: ( Pool pocket class. )
LIBRARY DEPENDENCY:
    (())
*******************************************************************************/
#ifndef _pocket_hh_
#define _pocket_hh_

#include <vector>
#include "common_geometry.hh"

class Pocket {
    public:
        // Have to have a default constructor or trick freaks out
        Pocket() {}
        Pocket(double x, double y, double r) : pos(x,y), radius(r) {}

        Vec pos;
        double radius;
    
};

#endif
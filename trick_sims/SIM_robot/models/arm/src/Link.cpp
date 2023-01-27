/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate balls contacting boundaries. )
LIBRARY DEPENDENCY:
    ((Link.o))
*******************************************************************************/
#include <math.h>
#include "../include/Link.hh"
#include "trick/memorymanager_c_intf.h"
#include <new>

Link::Link(double angle, double length) {
    q = angle;
    qdot = M_PI/12.0;
    l = length;
}

Link* make_Link( double angle, double length ) {
    Link* b = (Link*)TMM_declare_var_s("Link");
    return (new (b) Link(angle, length) );
}

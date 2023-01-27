/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate Robot Arm. )
LIBRARY DEPENDENCY:
    ((Link.o))
*******************************************************************************/
#ifndef _link_hh_
#define _link_hh_

class Link {
    public:
        Link(){}
        double l;
        double q;
        double qdot;

        Link( double angle, double length );
};

Link* make_Link( double angle, double length );

#endif

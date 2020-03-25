/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate balls contacting boundaries. )
LIBRARY DEPENDENCY:
    ((Ball.o))
*******************************************************************************/
#ifndef _ball_hh_
#define _ball_hh_

class Ball {
    public:
        Ball(){}
        double pos[2];
        double vel[2];
        double mass;
        double radius;
        Ball(double x, double y, double vx, double vy, double r, double m);
};

Ball* make_Ball(double x, double y, double vx, double vy, double r, double m);

#endif

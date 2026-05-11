/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate balls contacting boundaries. )
LIBRARY DEPENDENCY:
    ((Ball.o))
*******************************************************************************/
#ifndef _ball_hh_
#define _ball_hh_

#include <vector>

class Ball {
    public:
        Ball(){}

        std::vector<double> vecPos; 
        std::vector<double> vecVel; 

        double mass;
        double radius;
        Ball(double x, double y, double vx, double vy, double r, double m);

};

Ball* make_Ball(double x, double y, double vx, double vy, double r, double m);

#endif

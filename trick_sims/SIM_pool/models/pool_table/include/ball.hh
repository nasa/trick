/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate pool balls. )
LIBRARY DEPENDENCY:
    ((ball.o))
*******************************************************************************/
#ifndef _ball_hh_
#define _ball_hh_

// #include <Eigen/Core>

// monotonically increasing ID
static int id = 0;

class Ball {

    public:

        Ball(double x, double y, double mass, double radius, bool isFixed, int id);
        Ball () {}
        
        // Z component should always be 0, unless someone tries to add jumps in the future
        double pos[3];
        double prevPos[3];
        double vel[3];
        // Used to store derivatives between deriv and integration steps
        double accel[3];

        // Relating to angular velocity
        double relativeVel[3];
        double w[3];
        double angular_accel[3];

        double color[3];
    
        double mass;
        double radius;
        bool fixed;
        bool isCue;
        int sliding;

        unsigned int id;
};

// Ball* CreateBall(double x, double y, double mass, double radius, bool isFixed);

#endif
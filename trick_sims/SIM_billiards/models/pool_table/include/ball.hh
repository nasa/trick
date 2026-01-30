/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate pool balls. )
LIBRARY DEPENDENCY:
    ((ball.o))
*******************************************************************************/
#ifndef _ball_hh_
#define _ball_hh_

// #include <Eigen/Core>
#include "common_geometry.hh"

// monotonically increasing ID

class Ball
{
public:
    Ball(double x, double y, double mass, double radius, bool isFixed, int id);

    Ball() {}

    void setPos(double x, double y);
    void setPos(double x, double y, double z);

    void setVel(double x, double y);
    void setVel(double x, double y, double z);

    void setAccel(double x, double y);
    void setAccel(double x, double y, double z);

    // void setRelativeVel(double x, double y);
    void setRelativeVel(double x, double y, double z);

    // void setAngularVel(double x, double y);
    void setAngularVel(double x, double y, double z);

    // void setAngularAccel(double x, double y);
    void setAngularAccel(double x, double y, double z);

    void clearAllState();

    // Z component should always be 0, unless someone tries to add jumps in the future
    // double pos[3];
    // double prevPos[3]; // Maybe don't need this anymore?
    // double vel[3];
    // // Used to store derivatives between deriv and integration steps
    // double accel[3];

    // // Relating to angular velocity
    // double relativeVel[3];
    // double w[3];
    // double angular_accel[3];
    Vec pos;
    Vec prevPos;
    Vec vel;
    Vec accel;
    Vec relativeVel;
    Vec w;
    Vec angularAccel;

    // double color[3];

    double mass;
    double radius;
    bool fixed;
    bool isCue;
    int sliding;

    bool inPlay = true;

    unsigned int id;
};

// Ball* CreateBall(double x, double y, double mass, double radius, bool isFixed);

#endif

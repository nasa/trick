/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate balls contacting boundaries. )
LIBRARY DEPENDENCY:
    ((Ball.o))
*******************************************************************************/
#include "../include/Ball.hh"
#include "trick/memorymanager_c_intf.h"
#include <new>

Ball::Ball(double x, double y, double vx, double vy, double r, double m) {
    vecPos.push_back(x);
    vecPos.push_back(y);
    vecVel.push_back(vx);
    vecVel.push_back(vy);
    radius = r;
    mass = m;
}

Ball* make_Ball(double x, double y, double vx, double vy, double r, double m) {
    Ball* b = (Ball*)TMM_declare_var_s("Ball");
    return (new (b) Ball(x,y,vx,vy,r,m));
}

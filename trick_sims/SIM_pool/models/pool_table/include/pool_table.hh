/************************************************************************
PURPOSE: (Simulate a pool table.)
LIBRARY DEPENDENCIES:
    ((pool_table/src/pool_table.o))
**************************************************************************/

#ifndef _POOL_TABLE_H_
#define _POOL_TABLE_H_
#include "trick/regula_falsi.h"
#include "ball.hh"
#include "bumper.hh"
#include <vector>


class PoolTable {

    public:
        PoolTable () : numBalls(0), numAssociations(0) {}

        int addBall (double x, double y, double mass, double radius, bool fixed);
        int addBumper (double x1, double y1, double x2, double y2);

        int setBallPos(int id, double x, double y);
        int setBallVel(int id, double v_x, double v_y);

        // Ball ** balls;
        // Bumper ** bumpers;
        Ball** balls;
        Bumper** bumpers;

        // Ball-ball collisions
        int nextBallSlot = 0;
        unsigned int numBalls;
        unsigned int numAssociations;
        REGULA_FALSI* ballAssociations;

        // Ball-bumper collisions
        unsigned int numBumpers;
        unsigned int numCombos;
        REGULA_FALSI* bumperAssociations;        

        //void ballCollision(Ball &b1, Ball &b2);

        int default_data();
        int state_init();
        int state_deriv();
        int state_integ();
        double collision();

        // Sim constants that should be user-controllable
        double frictionRolling = 0.05;
        double frictionSliding = 0.25; 
        double frictionScale = 1;
        double frictionTolerance = 0.0005;
        double coefficientOfElasticity = 0.99;

};
#endif
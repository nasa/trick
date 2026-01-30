/************************************************************************
PURPOSE: (Simulate a pool table.)
LIBRARY DEPENDENCIES:
    ((pool_table/src/pool_table.o))
**************************************************************************/

#ifndef _POOL_TABLE_H_
#define _POOL_TABLE_H_
#include "ball.hh"
#include "bumper.hh"
#include "pocket.hh"
#include "trick/regula_falsi.h"
#include <vector>

class PoolTable
{
public:
    PoolTable()
        : numBalls(0),
          numAssociations(0)
    {
    }

    int default_data();
    int state_init();
    int state_deriv();
    int state_integ();
    double collision();
    // double bumperCollision();

    int addBall(double x, double y, double mass, double radius, bool fixed);
    int addBumper(int numPoints, double x1, double y1, double x2, double y2);
    int addPointToBumper(int id, double x, double y);
    int addPointToTable(double x, double y);
    int addPocket(double x, double y, double r);

    int setBallPos(int id, double x, double y);
    int setBallVel(int id, double v_x, double v_y);

    void applyCueForce(double x_end, double y_end);
    void applyCueForce(
        double x_end, double y_end, double cueHorizontalDisplacement, double cueVerticalDisplacement, double cueAngle);

    void resetCueBall(double x, double y);
    void resetCueBall();

    double removeBall(int id);

    // State variables
    Ball ** balls;

    // Table parameters
    // Bumpers and pockets are used by sim, tableShape is just used by graphics client
    Bumper ** bumpers;
    Pocket ** pockets;
    Vec ** tableShape;

    unsigned int numBumpers;
    unsigned int numPockets;
    unsigned int numTablePoints;

    int nextBallSlot = 0;
    int nextBumperSlot = 0;
    int nextPocketSlot = 0;
    int nextTablePointSlot = 0;

    enum PolygonType tableShapeType;

    // Ball-ball collisions
    unsigned int numBalls;
    unsigned int numAssociations;
    REGULA_FALSI * ballAssociations;

    // Ball-bumper collisions
    unsigned int bumperBallCombos;
    REGULA_FALSI * bumperAssociations;

    // Ball-pocket collisions
    unsigned int pocketBallCombos;
    REGULA_FALSI * pocketAssociations;

    // Sim constants that should be user-controllable
    double frictionRolling = 0.05;
    double frictionSliding = 0.25;
    double frictionScale = 1;
    double frictionTolerance = 0.0005;
    double coefficientOfElasticity = .95;
    double cueForceScale = 0.6;
    double cueMass = 1.0;

    int cueBallIndex = 0;
    double defaultCueBallX = -0.3;
    double defaultCueBallY = 0;

    bool allowCollisions = true;
};
#endif

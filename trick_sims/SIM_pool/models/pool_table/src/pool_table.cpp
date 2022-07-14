/********************************* TRICK HEADER *******************************
PURPOSE: (Simulate a pool table.)
LIBRARY DEPENDENCY:
    ((pool_table.o)
     (ball.o))
*******************************************************************************/
#include <math.h>
#include <iostream>
#include "trick/integrator_c_intf.h"
#include "trick/memorymanager_c_intf.h"
#include "../include/pool_table.hh"
#include "trick/trick_math_proto.h"

double dot(                  /* Return: Scalar dot or inner product */
                 double vec1[],        /* In: Vector 1 */
                 double vec2[],        /* In: Vector 2 */
                 int dim)
{                  
    double dot = 0;
    for (int i = 0; i < dim; i++) {
        dot += vec1[i] * vec2[i];
    }                    
    return dot;
}

void scaleInPlace (double vec[], double scale, int dim) {
    for (int i = 0; i < dim; i++) {
        vec[i] *= scale;
    }
}

void mProduct (double product[2], double matrix[2][2], double vec[2]) {
    product[0] = matrix[0][0] * vec[0] + matrix[1][0] * vec[1];
    product[1] = matrix[0][1] * vec[0] + matrix[1][1] * vec[1];
}




int PoolTable::default_data() {
    // balls.clear();
    // bumpers.clear();

    numBalls = 0;
    numBumpers = 0;
    return 0;
}

// Input stage runs before this, which populates balls and bumpers
int PoolTable::state_init() {
    
    // Do regula falsi setup here

    // Vars for ball/ball collisions
    double now ;
    numAssociations = (numBalls*(numBalls-1))/2;
    ballAssociations = (REGULA_FALSI*)TMM_declare_var_1d("REGULA_FALSI", numAssociations);
    unsigned int ii,jj;
    for (ii=1; ii<numBalls; ii++) {
        for (jj=0; jj<ii; jj++) {
            unsigned int association_index = ii*(ii-1)/2+jj;
            ballAssociations[association_index].mode = Decreasing;
            ballAssociations[association_index].error_tol = 0.0000001;
            now = get_integ_time() ;
            reset_regula_falsi( now, &ballAssociations[association_index] );
        }
    }

    bumperBallCombos = numBalls * numBumpers;
    bumperAssociations = (REGULA_FALSI*)TMM_declare_var_1d("REGULA_FALSI", bumperBallCombos);
    for (ii=0; ii<numBalls; ii++) {
        for (jj=0; jj<numBumpers; jj++) {
            unsigned int association_index = (ii*numBumpers) + jj;
            bumperAssociations[association_index].mode = Any;
            bumperAssociations[association_index].error_tol = 0.0000001;
            now = get_integ_time() ;
            reset_regula_falsi( now, &bumperAssociations[association_index] );
        }
    }

    // Need to do the same thing with pocket/ball associations
    pocketBallCombos = numBalls * numPockets;
    pocketAssociations = (REGULA_FALSI*)TMM_declare_var_1d("REGULA_FALSI", pocketBallCombos);
    for (ii=0; ii<numBalls; ii++) {
        for (jj=0; jj<numPockets; jj++) {
            unsigned int association_index = (ii*numPockets) + jj;
            pocketAssociations[association_index].mode = Decreasing;
            pocketAssociations[association_index].error_tol = 0.0000001;
            now = get_integ_time() ;
            reset_regula_falsi( now, &pocketAssociations[association_index] );
        }
    }

    return 0;
}

int PoolTable::state_deriv() {
    // derivative - determine the acceleration here
    // This is probably just friction
    // And angular stuff
    // F = MA => A = M^-1 * F

    // Will have to account for rolling vs sliding here
    // For now, just rolling

    for (int i = 0; i < numBalls; i++) {
        // Frictional force a constant applied in the opposing direction of velocity.
        // Magnitude of velocity is irrelevant unless very close to 0
        // Has weird behavior when velocity is very small, so only apply friction if velocity is greater than a tolerance

        if (!balls[i]->inPlay) {
            continue;
        }

        if (abs(dv_mag(balls[i]->vel)) > frictionTolerance) {
            double velocityNorm[3];
            dv_norm(velocityNorm, balls[i]->vel);

            balls[i]->accel[0] = - (frictionScale * frictionRolling * velocityNorm[0]);
            balls[i]->accel[1] = - (frictionScale * frictionRolling * velocityNorm[1]);
            balls[i]->accel[2] = 0;
        } else {
            balls[i]->vel[0] = 0;
            balls[i]->vel[1] = 0;
            balls[i]->vel[2] = 0;

            balls[i]->accel[0] = 0;
            balls[i]->accel[1] = 0;
            balls[i]->accel[2] = 0;

        }


    }

    return 0;
}

int PoolTable::state_integ() {
    // Apply the acceleration by changing the velocity by the appropriate amount over the time step 

    // How many state variables are needed for each ball
    int n = 6;

    for (int i = 0; i < numBalls; i++) {
        if (!balls[i]->inPlay) {
            continue;
        }

        // State - Need to load 4 values for each ball, but will have to have more when we add angular stuff
        // pos[0] pos[1] vel[0] vel[1]
        int inner_index = 0;
        load_indexed_state(n*i + inner_index++, balls[i]->pos[0]);
        load_indexed_state(n*i + inner_index++, balls[i]->pos[1]);
        load_indexed_state(n*i + inner_index++, balls[i]->pos[2]);

        load_indexed_state(n*i + inner_index++, balls[i]->vel[0]);
        load_indexed_state(n*i + inner_index++, balls[i]->vel[1]);
        load_indexed_state(n*i + inner_index++, balls[i]->vel[2]);

        // Derivatives of all of this junk
        // vel[0] vel[1] accel[0] accel[1]

        inner_index = 0;
        load_indexed_deriv(n*i + inner_index++, balls[i]->vel[0]);
        load_indexed_deriv(n*i + inner_index++, balls[i]->vel[1]);
        load_indexed_deriv(n*i + inner_index++, balls[i]->vel[2]);

        load_indexed_deriv(n*i + inner_index++, balls[i]->accel[0]);     // Needs to be accel[0]
        load_indexed_deriv(n*i + inner_index++, balls[i]->accel[1]);     // Needs to be accel[1]
        load_indexed_deriv(n*i + inner_index++, balls[i]->accel[2]);     // Needs to be accel[2]

    }

    int integration_step = integrate();

    for (int i = 0; i < numBalls; i++) {
        if (!balls[i]->inPlay) {
            continue;
        }

        // pos[0] pos[1] vel[0] vel[1]
        int inner_index = 0;
        balls[i]->pos[0] = unload_indexed_state(n*i + inner_index++);
        balls[i]->pos[1] = unload_indexed_state(n*i + inner_index++);
        balls[i]->pos[2] = unload_indexed_state(n*i + inner_index++);

        balls[i]->vel[0] = unload_indexed_state(n*i + inner_index++);
        balls[i]->vel[1] = unload_indexed_state(n*i + inner_index++);
        balls[i]->vel[2] = unload_indexed_state(n*i + inner_index++);

        balls[i]->accel[0] = 0;
        balls[i]->accel[1] = 0;
        balls[i]->accel[2] = 0;

    }

    return integration_step;
}

double closestPointOnLine(Line& line, double pos[2], double result[2], bool print) {
    double m[3];
    double a[3] = {line.p1.x, line.p1.y};
    double b[3] = {line.p2.x, line.p2.y};
    double diff[3];

    dv_sub(diff, pos, a);
    dv_sub(m, b, a);

    double t = dot(diff, m, 2) / dot(m, m, 2);

    // if (print)
    //     std::cout << "t: " << t << std::endl;

    if (t < 0)
        t = 0;
    
    if (t > 1)
        t = 1;

    scaleInPlace(m, t, 2);
    dv_add(result, a, m);
    // if (print)
    //     std::cout << "Result: " << result[0] << " " << result[1] << std::endl;

    return t;
}

double PoolTable::removeBall(int id) {
    balls[id]->inPlay = false;
    balls[id]->clearAllState();
}


double PoolTable::collision() {

    // Handle when the balls collide with others

    double now ; /* current integration time. */
    double event_tgo;
    unsigned int ii,jj;

    now = get_integ_time() ;
    event_tgo = BIG_TGO;

    std::vector<unsigned int> collisionsToProcess;

    for (ii=1; ii<numBalls; ii++) {
        if (!balls[ii]->inPlay) {
                continue;
        }
        for (jj=0; jj<ii; jj++) {
            if (!balls[jj]->inPlay) {
                continue;
            }
            
            double diff[3];
            dv_sub(diff, balls[ii]->pos, balls[jj]->pos);
            double distanceBetweenBalls = dv_mag(diff);
            unsigned int associationIndex = ii*(ii-1)/2+jj;

            // boundary is distance between balls - radiuses of balls
            double error = distanceBetweenBalls - (balls[ii]->radius + balls[jj]->radius);

            // std::cout << "Distance between centers: " << distanceBetweenBalls << std::endl;
            // std::cout << "Radiuses: " << balls[ii]->radius << " " << balls[jj]->radius << std::endl;
            // std::cout << "Error calculation: " << error << std::endl;
            ballAssociations[associationIndex].error = error;
            double this_tgo = regula_falsi( now, &(ballAssociations[associationIndex])) ;
            
            if (this_tgo < event_tgo) {
                event_tgo = this_tgo;
            }

            if (this_tgo == 0) {
                std::cout << "Found colliding balls" << std::endl;

                // Add this collision to a list of collisions to process
                collisionsToProcess.push_back(ii);
                collisionsToProcess.push_back(jj);
                reset_regula_falsi( now, &(ballAssociations[associationIndex]) );

            } else {
                regula_falsi_set_upper (now, error, &ballAssociations[associationIndex]);
            }
        }
    }

    // Handle collisions
    for (int i = 0; i < collisionsToProcess.size(); i+=2) {
        // if (allowCollisions == false) {
        //     return event_tgo;
        // }
        // allowCollisions = false;
        int index1 = collisionsToProcess[i];
        int index2 = collisionsToProcess[i+1];
        std::cout << "Collision detected between balls " << index1 << " and " << index2 << std::endl;

        double q1[3];
        dv_copy(q1, balls[index1]->pos);
        double q2[3];
        dv_copy(q2, balls[index2]->pos);


        // dg = (q1 - q2) / (|q1 - q2|)
        double diff[3];
        dv_sub(diff, q1, q2);
        double dg[3];
        dv_norm(dg, diff);

        // Have to stuff both velocities and dg values into 4d vector to do the calculation
        // Otherwise I have to do more math
        double dg4[4] = {dg[0], dg[1], -dg[0], -dg[1]};
        double vel4[4] = {balls[index1]->vel[0], balls[index1]->vel[1], balls[index2]->vel[0], balls[index2]->vel[1]};

        // Calculate the impulse
        // J = ((-(1 + c) * dg * v) / (dg * M^-1 * dg^T) ) dg
        // For now let's just pretend all the masses are 1
        double impulse = ((1.0 + coefficientOfElasticity) * dot(dg4, vel4, 4)) / (dot(dg4, dg4, 4));
        scaleInPlace(dg4, impulse, 4);

        // Impulse[0:1] is x and y components of what should be applied to v1, Impulse[2:3] goes to v2
        double impulse1[3] = {dg4[0], dg4[1], 0};
        double impulse2[3] = {dg4[2], dg4[3], 0};

        double newV1[3];
        dv_copy(newV1, balls[index1]->vel);
        double newV2[3];
        dv_copy(newV2, balls[index2]->vel);

        scaleInPlace(newV1, balls[index1]->mass, 3);
        scaleInPlace(newV2, balls[index2]->mass, 3);

        dv_sub(newV1, newV1, impulse1);
        dv_sub(newV2, newV2, impulse2);

        scaleInPlace(newV1, 1.0 / balls[index1]->mass, 3);
        scaleInPlace(newV2, 1.0 / balls[index2]->mass, 3);

        std::cout << "Impulses applied: \n\tV1: " << newV1[0] << " " << newV1[1] << " \n\tV2: " << newV2[0] << " " << newV2[1] << std::endl;

        balls[index1]->vel[0] = newV1[0];
        balls[index1]->vel[1] = newV1[1];

        balls[index2]->vel[0] = newV2[0];
        balls[index2]->vel[1] = newV2[1];
    }

    // std::cout << "Ball 0 pos: " << balls[0]->pos[0] << " " << balls[0]->pos[1] << std::endl;
    // std::cout << "Bumper 0 pos: " << bumpers[0]->border.p1.x << " " << bumpers[0]->border.p1.y << " " << bumpers[0]->border.p2.x << " " << bumpers[0]->border.p2.y << std::endl;

    int numBumperCollisions = 0;
    for (ii=0; ii<numBalls; ii++) {
        if (!balls[ii]->inPlay) {
                continue;
        }
        // if (numBumperCollisions > 0)
        //     break;
        for (jj=0; jj<numBumpers; jj++) {
            // if (numBumperCollisions > 0)
            //     break;
            unsigned int association_index = (ii*numBumpers) + jj;
            Ball *ball = balls[ii];
            Bumper *bumper = bumpers[jj];
            //Point ballPos(ball->pos[0], ball->pos[1]);
            double closestPointOnBumper[3] = {0, 0, 0};
            if (ii == 0 && jj == 3) {
                double t = closestPointOnLine(bumper->border, ball->pos, closestPointOnBumper, true);
                // std::cout << "Closest point on bumper: " << closestPointOnBumper[0] << " " << closestPointOnBumper[1] << std::endl;

            } else {
                double t = closestPointOnLine(bumper->border, ball->pos, closestPointOnBumper, false);
            }
            double diff[3];

            // dv_sub(diff, ball->pos, closestPointOnBumper);
            diff[0] = ball->pos[0] - closestPointOnBumper[0];
            diff[1] = ball->pos[1] - closestPointOnBumper[1];
            diff[2] = 0;

            double distanceToBumper = abs(dv_mag(diff)) - ball->radius;

            bumperAssociations[association_index].error = distanceToBumper;
            // if (ii == 0 && jj == 3) {
            //     std::cout << "Ball pos: " << balls[ii]->pos[0] << " " << balls[ii]->pos[1] << std::endl;
            //     std::cout << "Closest point on bumper: " << closestPointOnBumper[0] << " " << closestPointOnBumper[1] << std::endl;
            //     std::cout << "Distance to bumper: " << bumperAssociations[association_index].error << std::endl;
            // }
            double this_tgo = regula_falsi( now, &(bumperAssociations[association_index])) ;
            if (this_tgo < event_tgo) {
                event_tgo = this_tgo;
            }

            if (this_tgo == 0) {
                numBumperCollisions++;
                std::cout << "Found colliding ball " << ii << " and bumper " << jj << std::endl;

                reset_regula_falsi( now, &(bumperAssociations[association_index]) );

                if (numBumperCollisions > 1) 
                    continue;
                    
                double q1[3];
                dv_copy(q1, ball->pos);
                double q2[3];
                dv_copy(q2, closestPointOnBumper);
                q1[2] = 0;
                q2[2] = 0;


                // dg = (q1 - q2) / (|q1 - q2|)
                double diff[3];
                dv_sub(diff, q1, q2);
                double dg[3];
                dv_norm(dg, diff);

                // Have to stuff both velocities and dg values into 4d vector to do the calculation
                // Otherwise I have to do more math
                double dg4[4] = {dg[0], dg[1], -dg[0], -dg[1]};
                double vel4[4] = {ball->vel[0], ball->vel[1], 0, 0};

                // Calculate the impulse
                // J = ((-(1 + c) * dg * v) / (dg * M^-1 * dg^T) ) dg
                // Really need to get some better matrix math in here
                // only used the dot product of the first 2 in the last term since it's supposed to be dg * minv * dg
                // and since the bumpers are immovable the mass is infinite which inverse is 0
                // So this isn't robust at all
                // Setting the CoE to be 1 always - perfectly elastic
                // Otherwise there's a weird bouncy thing
                double impulse = ((1.0 + 1.0) * dot(dg4, vel4, 4)) / (dot(dg4, dg4, 2));
                std::cout << "Impulse: " << impulse << std::endl;
                scaleInPlace(dg4, impulse, 4);

                // Impulse[0:1] is x and y components of what should be applied to v1, Impulse[2:3] goes to v2
                double impulse1[3] = {dg4[0], dg4[1], 0};
                double impulse2[3] = {dg4[2], dg4[3], 0};

                double newV1[3];
                dv_copy(newV1, ball->vel);
                // double newV2[3] = {0, 0};

                scaleInPlace(newV1, ball->mass, 3);
                // scaleInPlace(newV2, balls[index2]->mass, 3);

                dv_sub(newV1, newV1, impulse1);
                // dv_sub(newV2, newV2, impulse2);

                scaleInPlace(newV1, 1.0 / ball->mass, 3);
                // scaleInPlace(newV2, 1.0 / balls[index2]->mass, 3);

                std::cout << "Impulses applied: \n\tV1: " << newV1[0] << " " << newV1[1] << std::endl;

                ball->vel[0] = newV1[0];
                ball->vel[1] = newV1[1];

            } else {
                regula_falsi_set_upper (now, distanceToBumper, &bumperAssociations[association_index]);
            }
        }

    }
    if (numBumperCollisions > 0) {
        std::cout << "In this time step, " << numBumperCollisions << " collisions applied" << std::endl;
    }

    // Pockets
    for (ii=0; ii<numBalls; ii++) {
        if (!balls[ii]->inPlay) {
                continue;
        }
        for (jj=0; jj<numPockets; jj++) {
            unsigned int association_index = (ii*numPockets) + jj;
            Ball *ball = balls[ii];
            Pocket *pocket = pockets[jj];
            double pocketPos[3] = {pocket->x, pocket->y, 0};

            double diff[3];
            dv_sub(diff, ball->pos, pocketPos);
            double distance = dv_mag(diff);

            // Ball is pocketed when center is within pocket radius
            double error = distance - (pocket->radius);

            pocketAssociations[association_index].error = error;
            double this_tgo = regula_falsi( now, &(pocketAssociations[association_index]));

            if (this_tgo < event_tgo) {
                event_tgo = this_tgo;
            }

            if (this_tgo == 0) {
                std::cout << "Found pocketed ball" << std::endl;
                reset_regula_falsi( now, &(pocketAssociations[association_index]) );
                removeBall(ii);
            }
        }
    }

    return event_tgo;
}

void PoolTable::resetCueBall() {
    resetCueBall(defaultCueBallX, defaultCueBallY);
}

void PoolTable::resetCueBall(double x, double y) {
    balls[cueBallIndex]->clearAllState();
    balls[cueBallIndex]->setPos(x, y);

    balls[cueBallIndex]->inPlay = true;
}



void PoolTable::applyCueForce(double x_end, double y_end) {
    applyCueForce(x_end, y_end, 0, 0, 0);
}


void PoolTable::applyCueForce(double x_end, double y_end, double cueHorizontalDisplacement, double cueVerticalDisplacement, double cueAngle) {
    std::cout << "Applying cue force" << std::endl;

    // assume index 0 is cue ball
    int cueIndex = 0;
    double cueEnd[3] = {x_end, y_end};
    double cueBallPos[3] = {balls[cueIndex]->pos[0], balls[cueIndex]->pos[1]};

    double r = balls[cueIndex]->radius;
    double m = balls[cueIndex]->mass;
    
    double a = cueHorizontalDisplacement * r;
    double b = cueVerticalDisplacement * r;
    double c = abs(sqrt(pow(r,2) - pow(a,2) - pow(b,2)));

    double theta = cueAngle * M_PI / 180.0;
    double angleScaling = 1.0 + (cueAngle / 25.0);

    double cueAxis[3];
    dv_sub(cueAxis, cueEnd, cueBallPos);
    double cue_v = dv_mag(cueAxis) * cueForceScale * angleScaling;
    dv_norm(cueAxis, cueAxis);
    double axis2[2] = {cueAxis[0], cueAxis[1]};

    double force = (2 * m * cue_v) / (1.0 * (m / cueMass) + (5.0 / (2.0*pow(r, 2))) * (pow(a,2) + pow(b,2)*pow(cos(theta),2) + pow(c,2)*pow(sin(theta),2) - 2*b*c*sin(theta)*cos(theta)));

    // Velocity in the frame of the cue
    double v_b[2] = {0, -force/m * cos(theta)};

    // Rotate to table frame
    double y_unit[2] = {0, 1};
    double rot = acos(dot(axis2, y_unit, 2) / 1.0);
    if (cueAxis[0] < 0) {
        rot = -rot;
    }

    double rotationMatrix[2][2];
    rotationMatrix[0][0] = cos(rot);
    rotationMatrix[0][1] = -sin(rot);
    rotationMatrix[1][0] = sin(rot);
    rotationMatrix[1][1] = cos(rot);

    double v_table[2];
    mProduct(v_table, rotationMatrix, v_b);

    std::cout << "Velocity of ball after cue hit: " << v_table[0] << " " << v_table[1] << std::endl;

    balls[cueIndex]->vel[0] = v_table[0];
    balls[cueIndex]->vel[1] = v_table[1];

    // TODO: logic for angular and relative velocity

}


// Add a ball to the table and return the ID
int PoolTable::addBall (double x, double y, double mass, double radius, bool fixed) {
    int id = nextBallSlot++;
    if (id < numBalls) {
        Ball * ball = (Ball*) TMM_declare_var_s("Ball");
        balls[id] = (new (ball) Ball(x,y,mass,radius, fixed, id));
        return id;
    }
    return -1;
}

// Add a bumper to the table and return the ID
// Only takes in the actual effective line, need to add something else for the rendered shape
int PoolTable::addBumper (int numPoints, double x1, double y1, double x2, double y2) {
    int id = nextBumperSlot++;
    if (id < numBumpers) {
        Bumper * bumper = (Bumper*) TMM_declare_var_s("Bumper");
        bumpers[id] = (new (bumper) Bumper(numPoints, x1, y1, x2, y2));
        return id;
    }
    return -1;
}

int PoolTable::addPointToBumper(int id, double x, double y) {
    Bumper * bumper = bumpers[id];
    bumper->AddPointToRender(x, y);

    return 0;
}

int PoolTable::addPointToTable(double x, double y) {
    int id = nextTablePointSlot++;
    if (id < numTablePoints) {
        Point * point = (Point*) TMM_declare_var_s("Point");
        tableShape[id] = (new (point) Point(x, y));
        return id;
    }
    return -1;
}

int PoolTable::addPocket(double x, double y, double r) {
    int id = nextPocketSlot++;
    if (id < numPockets) {
        Pocket * pocket = (Pocket*) TMM_declare_var_s("Pocket");
        pockets[id] = (new (pocket) Pocket(x, y, r));
        return id;
    }
    return -1;
}



int PoolTable::setBallPos(int id, double x, double y) {
    if (id < numBalls && balls[id] != NULL) {
        balls[id]->pos[0] = x;
        balls[id]->pos[1] = y;
        return 1;
    }

    return -1;
}

int PoolTable::setBallVel(int id, double v_x, double v_y) {
    if (id < numBalls && balls[id] != NULL) {
        balls[id]->vel[0] = v_x;
        balls[id]->vel[1] = v_y;
        return 1;
    }
    return -1;
}




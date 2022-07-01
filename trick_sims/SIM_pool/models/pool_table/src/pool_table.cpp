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




int PoolTable::default_data() {
    // balls.clear();
    // bumpers.clear();


    // Dev Testing only - should be deleted
    // int id1 = addBall(0.5, 0.5, 1, 1, false);
    // int id2 = addBall(0.75, 0.5, 1, 1, false);

    // setBallVel(id1, 0.1, 0);
    ///////////////////////////////////////////

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

    // Need to do the same thing with rail/ball associations

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
        // Magnitude of velocity is irrelevant

        // std::cout << "ActuaVelocity: " << balls[i]->vel[0] << " " << balls[i]->vel[1] << std::endl;
        // std::cout << "Velocity Norm: " << velocityNorm[0] << " " << velocityNorm[1] << std::endl;

        // balls[i]->accel[0] = 0;
        // balls[i]->accel[1] = 0;

        // Has weird behavior when velocity is very small, so only apply friction if velocity is greater than a tolerance
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
        // pos[0] pos[1] vel[0] vel[1]
        int inner_index = 0;
        balls[i]->pos[0] = unload_indexed_state(n*i + inner_index++);
        balls[i]->pos[1] = unload_indexed_state(n*i + inner_index++);
        balls[i]->pos[2] = unload_indexed_state(n*i + inner_index++);

        balls[i]->vel[0] = unload_indexed_state(n*i + inner_index++);
        balls[i]->vel[1] = unload_indexed_state(n*i + inner_index++);
        balls[i]->vel[2] = unload_indexed_state(n*i + inner_index++);
    }

    return 0;
}

// Maybe need a separate scheduled job to handle pockets?
// And the cue?
// Maybe see if there's some "callback" that can handle user input
// There must be, since other sims have control panels too

double PoolTable::collision() {
    // Handle when the balls collide with others or with a bumper

    double now ; /* current integration time. */
    unsigned int first, second;

    unsigned int association_index;
    double event_tgo;
    unsigned int ii,jj;

    now = get_integ_time() ;
    event_tgo = BIG_TGO;

    std::vector<unsigned int> collisionsToProcess;

    for (ii=1; ii<numBalls; ii++) {
        for (jj=0; jj<ii; jj++) {
            double diff[3];
            dv_sub(diff, balls[ii]->pos, balls[jj]->pos);
            double distanceBetweenBalls = dv_mag(diff);
            unsigned int associationIndex = ii*(ii-1)/2+jj;

            // boundary is distance between balls - radiuses of balls
            ballAssociations[associationIndex].error = distanceBetweenBalls - (balls[ii]->radius + balls[jj]->radius);
            double this_tgo = regula_falsi( now, &(ballAssociations[associationIndex])) ;
            
            if (this_tgo < event_tgo) {
                event_tgo = this_tgo;
            }

            if (this_tgo == 0) {
                // Add this collision to a list of collisions to process
                collisionsToProcess.push_back(ii);
                collisionsToProcess.push_back(jj);
            }
        }
    }

    // Handle collisions
    for (int i = 0; i < collisionsToProcess.size(); i+=2) {
        int index1 = collisionsToProcess[i];
        int index2 = collisionsToProcess[i+1];

        double *q1 = balls[index1]->pos;
        double *q2 = balls[index2]->pos;


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
        scaleInPlace(dg, impulse, 4);

        

    }

    return event_tgo;
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
int PoolTable::addBumper (double x1, double y1, double x2, double y2) {
    // bumpers.push_back(bumper);
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




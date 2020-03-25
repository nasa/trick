/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate balls contacting boundaries. )
LIBRARY DEPENDENCY:
    ((Contact.o)
     (Ball.o))
*******************************************************************************/
#include <math.h>
#include <iostream>
#include "trick/integrator_c_intf.h"
#include "../include/Contact.hh"

int Contact::default_data() {
    balls = NULL;
    nballs = 0;
    return (0);
}

int Contact::state_init() {
    rf.mode = Decreasing;
    rf.error_tol = 0.001;
    return (0);
}

int Contact::state_deriv() {
    return(0);
}

int Contact::state_integ() {
    int integration_step;
    for (unsigned int ii = 0; ii < nballs; ii++) {
        // Be sure to fix load_indexed_state() so it only loads @ intermediate_step == 0
        load_indexed_state( 2*ii,   balls[ii]->pos[0]);
        load_indexed_state( 2*ii+1, balls[ii]->pos[1]);
    }
    for (unsigned int ii = 0; ii < nballs; ii++) {
        load_indexed_deriv( 2*ii,   balls[ii]->vel[0]);
        load_indexed_deriv( 2*ii+1, balls[ii]->vel[1]);
    }
    integration_step = integrate();
    for (unsigned int ii = 0; ii < nballs; ii++) {
        balls[ii]->pos[0] = unload_indexed_state( 2*ii );
        balls[ii]->pos[1] = unload_indexed_state( 2*ii+1 );
    }
    return(integration_step);
}

void Contact::ballCollision(Ball &b1, Ball &b2) {
    double unorm[2];
    double utang[2];
    if ((b1.mass > 0.0) && (b2.mass > 0.0)) {
        unorm[0] = ( b2.pos[0] - b1.pos[0] ) / ( b2.radius + b1.radius );
        unorm[1] = ( b2.pos[1] - b1.pos[1] ) / ( b2.radius + b1.radius );
        utang[0] = -unorm[1];
        utang[1] =  unorm[0];
        double b1vn = unorm[0] * b1.vel[0] + unorm[1] * b1.vel[1];
        double b1vt = utang[0] * b1.vel[0] + utang[1] * b1.vel[1];
        double b2vn = unorm[0] * b2.vel[0] + unorm[1] * b2.vel[1];
        double b2vt = utang[0] * b2.vel[0] + utang[1] * b2.vel[1];
        double b1vtAfter = b1vt;
        double b2vtAfter = b2vt;
        double b1vnAfter = ( b1vn * (b1.mass - b2.mass) + 2.0 * b2.mass * b2vn ) / ( b1.mass + b2.mass );
        double b2vnAfter = ( b2vn * (b2.mass - b1.mass) + 2.0 * b1.mass * b1vn ) / ( b1.mass + b2.mass );
        b1.vel[0] = b1vnAfter * unorm[0] + b1vtAfter * utang[0];
        b1.vel[1] = b1vnAfter * unorm[1] + b1vtAfter * utang[1];
        b2.vel[0] = b2vnAfter * unorm[0] + b2vtAfter * utang[0];
        b2.vel[1] = b2vnAfter * unorm[1] + b2vtAfter * utang[1];
    } else {
        std::cout << "ERROR: Balls must have positive mass." << std::endl;
    }
}

double Contact::collision() {
    double tgo ; /* time-to-go */
    double now ; /* current integration time. */
    int first, second;

    double min_dist = 1000000.0;
    for (unsigned int ii = 0; ii < nballs; ii++) {
        for (unsigned int jj = ii+1; jj < nballs; jj++) {
            double xdiff = balls[ii]->pos[0] - balls[jj]->pos[0];
            double ydiff = balls[ii]->pos[1] - balls[jj]->pos[1];
            double dist = sqrt( xdiff * xdiff + ydiff * ydiff) - (balls[ii]->radius + balls[jj]->radius);
            if (dist < min_dist) {
                min_dist = dist;
                first = ii;
                second = jj;
            }
        }
    }
    rf.error = min_dist;

    now = get_integ_time() ;               /* Get the current integration time */
    tgo = regula_falsi( now, &rf ) ;       /* Estimate remaining integration time. */
    if (tgo == 0.0) {                      /* If we are at the event, it's action time! */
        now = get_integ_time() ;
        ballCollision(*balls[first], *balls[second]);
        std::cout << "Ball["<<first<<"] and Ball["<<second<<"] collide at t = "<<now<<"." << std::endl;
        reset_regula_falsi( now, &rf );
    }
    return (tgo) ;
}

/********************************* TRICK HEADER *******************************
PURPOSE: ( Simulate balls contacting boundaries. )
LIBRARY DEPENDENCY:
    ((Contact.o)
     (Ball.o))
*******************************************************************************/
#include <math.h>
#include <iostream>
#include "trick/integrator_c_intf.h"
#include "trick/memorymanager_c_intf.h"
#include "../include/Contact.hh"

int Contact::default_data() {
    balls = NULL;
    nballs = 0;
    return (0);
}

int Contact::state_init() {

    double now ;
    numAssociations = (nballs*(nballs-1))/2;
    ballAssociations = (REGULA_FALSI*)TMM_declare_var_1d("REGULA_FALSI", numAssociations);
    unsigned int ii,jj;
    for (ii=1; ii<nballs; ii++) {
        for (jj=0; jj<ii; jj++) {
            unsigned int association_index = ii*(ii-1)/2+jj;
            ballAssociations[association_index].mode = Decreasing;
            ballAssociations[association_index].error_tol = 0.0000001;
            now = get_integ_time() ;
            reset_regula_falsi( now, &ballAssociations[association_index] );
        }
    }
    std::cout << "Ball\tPos[0]\tPos[1]\tVel[0]\tVel[1]" << std::endl;
    return (0);
}

int Contact::state_deriv() {
    return(0);
}

void Contact::state_print()
{
    for(int i  = 0; i < nballs; i++)
    {
        std::cout << i <<"\t"<<balls[i]->vecPos[0]<<"\t"<<balls[i]->vecPos[1]<<"\t"<<balls[i]->vecVel[0]<<"\t"<<balls[i]->vecVel[1]<<std::endl;
    }
}

int Contact::state_integ() {
    int integration_step;

    std::vector<double*> all_pos;
    std::vector<double*> all_vel;

    all_pos.reserve(nballs * 2);
    all_vel.reserve(nballs * 2);

    for (unsigned int ii = 0; ii < nballs; ii++)
    {
        for (unsigned int jj = 0; jj < balls[ii]->vecPos.size(); jj++)
        {
            all_pos.push_back(&balls[ii]->vecPos[jj]);
            all_vel.push_back(&balls[ii]->vecVel[jj]);
        }
    }

    load_state_vec(all_pos);
    load_deriv_vec(all_vel);

    integration_step = integrate();

    unload_state_vec(all_pos);


    return(integration_step);
}

void Contact::ballCollision(Ball &b1, Ball &b2) {
    double unorm[2];
    double utang[2];
    if ((b1.mass > 0.0) && (b2.mass > 0.0)) {
        unorm[0] = ( b2.vecPos[0] - b1.vecPos[0] ) / ( b2.radius + b1.radius );
        unorm[1] = ( b2.vecPos[1] - b1.vecPos[1] ) / ( b2.radius + b1.radius );
        utang[0] = -unorm[1];
        utang[1] =  unorm[0];
        double b1vn = unorm[0] * b1.vecVel[0] + unorm[1] * b1.vecVel[1];
        double b1vt = utang[0] * b1.vecVel[0] + utang[1] * b1.vecVel[1];
        double b2vn = unorm[0] * b2.vecVel[0] + unorm[1] * b2.vecVel[1];
        double b2vt = utang[0] * b2.vecVel[0] + utang[1] * b2.vecVel[1];
        double b1vtAfter = b1vt;
        double b2vtAfter = b2vt;
        double b1vnAfter = ( b1vn * (b1.mass - b2.mass) + 2.0 * b2.mass * b2vn ) / ( b1.mass + b2.mass );
        double b2vnAfter = ( b2vn * (b2.mass - b1.mass) + 2.0 * b1.mass * b1vn ) / ( b1.mass + b2.mass );
        b1.vecVel[0] = b1vnAfter * unorm[0] + b1vtAfter * utang[0];
        b1.vecVel[1] = b1vnAfter * unorm[1] + b1vtAfter * utang[1];
        b2.vecVel[0] = b2vnAfter * unorm[0] + b2vtAfter * utang[0];
        b2.vecVel[1] = b2vnAfter * unorm[1] + b2vtAfter * utang[1];
    } else {
        std::cout << "ERROR: Balls must have positive mass." << std::endl;
    }
}

double Contact::collision() {
    //double tgo ; /* time-to-go */
    double now ; /* current integration time. */
    unsigned int first, second;

    unsigned int association_index;
    double event_tgo;
    unsigned int ii,jj;

    now = get_integ_time() ;
    event_tgo = BIG_TGO;

    for (ii=1; ii<nballs; ii++) {
        for (jj=0; jj<ii; jj++) {
            association_index = ii*(ii-1)/2+jj;
            double xdiff = balls[ii]->vecPos[0] - balls[jj]->vecPos[0];
            double ydiff = balls[ii]->vecPos[1] - balls[jj]->vecPos[1];
            double distance = sqrt( xdiff * xdiff + ydiff * ydiff) - (balls[ii]->radius + balls[jj]->radius);
            ballAssociations[association_index].error = distance;
            double tgo = regula_falsi( now, &ballAssociations[association_index] ) ;

            if (tgo < BIG_TGO) {
                first = ii;
                second = jj;
                event_tgo = tgo;
            }
        }
    }
    if (event_tgo == 0.0) {
        now = get_integ_time() ;
        ballCollision(*balls[first], *balls[second]);
        std::cout << "Balls "<<second<<" and "<<first<<" collide at t = "<<now<<"." << std::endl;
        //
        for (ii=1; ii<nballs; ii++) {
            for (jj=0; jj<ii; jj++) {
                association_index = ii*(ii-1)/2+jj;
                if ((ii==first) && (jj==second)) {
                    reset_regula_falsi( now, &ballAssociations[association_index] );
                } else {
                    // Update the remaining REGULA_FALSI objects to the current event state.
                    double xdiff = balls[ii]->vecPos[0] - balls[jj]->vecPos[0];
                    double ydiff = balls[ii]->vecPos[1] - balls[jj]->vecPos[1];
                    double distance = sqrt( xdiff * xdiff + ydiff * ydiff) - (balls[ii]->radius + balls[jj]->radius);
                    regula_falsi_set_upper (now, distance, &ballAssociations[association_index]);
                }
            }
        }
    }
    return (event_tgo) ;
}

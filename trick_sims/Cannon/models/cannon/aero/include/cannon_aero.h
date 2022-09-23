/****************************** TRICK HEADER ******************************
PURPOSE: ( Define CANNON_AERO type. )
Tutorial Sections 8, and 9
***************************************************************************/

#ifndef CANNON_AERO_H
#define CANNON_AERO_H

#include "sim_services/Integrator/include/regula_falsi.h"
#include "sim_services/include/Flag.h"

typedef enum {
        Hard_Coded_Coefficient_Lift,   /* You come up with Cl  */
        Smits_Smith,                   /* Cl ~= 0.54*S^0.4 (S = rw/V) */
        Adair_Giordano,                /* Lift_Force = mass*0.00041*w_cross_V */
        Tombras                        /* Cl = 1/(2.022 + 0.981v/w) */
} Lift_Estimation_Method ;

typedef struct {
        double pos[3] ;           /* m position */
        double vel[3] ;           /* m/s velocity */
        double acc[3] ;           /* m/s2 acceleration */
        double omega[3] ;         /* rad/s Angular velocity of cannonball */

        double theta ;            /* rad Angle from x-axis to axis rotation */
        double phi ;              /* rad Angle from z-axis to axis rotation */
        double omega0 ;           /* rad/s Magnitude of angular velocity about
                                           axis of rotation */
        /* Impact */
        REGULA_FALSI rf ;         /* -- Dynamic event params for impact */
        int impact ;              /* -- Has impact occured */
        double impact_pos ;       /* m How far ball lands in field */

        /* Forces */
        double force_gravity[3] ; /* N Gravitational force */
        double force_drag[3] ;    /* N Drag force opposite dir of velocity */
        double force_magnus[3] ;  /* N Force due to spin, dir grav cross drag */
        double force_cross[3] ;   /* N Side directional force */
        double force_total[3] ;   /* N Sum of all forces */

        /* Force magnitudes */
        double mag_force_drag ;    /* N Magnitude of drag force */
        double mag_force_magnus ;  /* N Magnitude of magnus force */
        double mag_force_cross ;   /* N Magnitude of cross force */
        double mag_omega ;         /* rad/s Magnitude of angular velocity */

        void ** force_collect ;     /* ** For collect statement */

        /* Environment and Properties */
        double mass ;              /* kg Mass of cannonball */
        double air_density ;       /* kg/m3 Air density at 20C */
        double ball_radius ;       /* m Radius of cannonball */
        double ball_area ;         /* m2 Cross sectional area of ball */
        double spin_parameter ;    /* -- S=r*omega/speed */
        double g ;                 /* m/s2 Gravitational acceleration */

        /* Coefficients drag, lift and cross */
        Lift_Estimation_Method  lift_method ; /* -- How to find lift force */
        double coefficient_drag ;             /* -- Drag coefficient */
        double coefficient_lift ;             /* -- Lift coefficient */
        double coefficient_cross ;            /* -- Cross-Force coefficient */

        /* Jet (Tutorial Section 9) */
        int jet_on ;           /* -- 0|1 */
        int jet_count ;        /* -- How many jet firings? */
        double force_jet[3] ;  /* N Jet force per firing */
        double force_jet_Z_plus ; /* N Configurable force of jet in Z+ direction */

        /* Firing the Jet for Monte Carlo Runs */
        double time_to_fire_jet_1 ; /* s First jet firing time */
        double time_to_fire_jet_2 ; /* s Second jet firing time */
        double time_to_fire_jet_3 ; /* s Third jet firing time */
        double time_to_fire_jet_4 ; /* s Fourth jet firing time */

} CANNON_AERO ;
#endif

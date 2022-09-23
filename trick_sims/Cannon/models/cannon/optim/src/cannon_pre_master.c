/*********************************** TRICK HEADER **************************
PURPOSE:                     (Master optimization)
LIBRARY_DEPENDENCY:          (amoeba.o)
PROGRAMMER:                  (keith)
***************************************************************************/
#include <stdio.h>
#include "cannon/aero/include/cannon_aero.h"
#include "../include/amoeba.h"
#include "../include/amoeba_proto.h"
#include "sim_services/UnitTest/include/trick_tests.h"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/MonteCarlo/include/montecarlo_c_intf.h"

int cannon_pre_master(
                            /* RETURN: -- Always return zero */
  CANNON_AERO* C,           /* INOUT:  -- Parameter */
  AMOEBA* A)
{

        while ( 1 ) {

                switch ( A->state ) {

                    case VERTICES:
                        A->curr_point = A->vertices[A->curr_vertex] ;
                        if ( A->curr_vertex == A->num_vertices ) {
                                A->state = CALC_CENTROID_POINT ;
                        } else {
                                fprintf(stderr, "V[%d] ", A->curr_vertex);
                        }
                        A->curr_vertex++ ;
                        break ;

                    case CALC_CENTROID_POINT:
                        fprintf(stderr, "CENT ");
                        amoeba_order( A ) ;
                        A->curr_point = A->x_cent ;
                        A->state = CALC_REFLECTION_POINT ;
                        break ;

                    case CALC_REFLECTION_POINT:
                        fprintf(stderr, "REFL ");
                        amoeba_calc_reflection_point( A ) ;
                        A->curr_point = A->x_refl ;
                        A->state = REFLECT ;
                        break ;

                    case REFLECT:
                        if ( amoeba_reflect( A ) ) {
                                A->state = CALC_CENTROID_POINT ;
                        } else {
                                fprintf(stderr, "EXPA ");
                                amoeba_calc_expansion_point( A ) ;
                                A->curr_point = A->x_expa ;
                                A->state = EXPAND ;
                        }
                        break ;

                    case EXPAND:
                        if ( amoeba_expand( A ) ) {
                                A->state = CALC_CENTROID_POINT ;
                        } else {
                                fprintf(stderr, "CONT ");
                                amoeba_calc_contraction_point( A ) ;
                                A->curr_point = A->x_cont ;
                                A->state = CONTRACT ;
                        }
                        break ;

                    case CONTRACT:
                        if ( amoeba_contract( A ) ) {
                                A->state = CALC_CENTROID_POINT ;
                        } else {
                                A->state = SHRINK ;
                        }
                        break ;

                    case SHRINK:
                        amoeba_shrink( A ) ;
                        fprintf(stderr, "V[0] ");
                        A->curr_point = A->vertices[0] ;
                        A->curr_vertex = 1 ;
                        A->state = VERTICES ;
                        break ;
                }

                if ( amoeba_satisfied( A ) && A->state != VERTICES ) {
                        /* This is for SRS */
                        if (mc_get_num_results() == 66) {
                            add_test_result("Sim_Amoeba Simulation", "MonteCarlo Optimization", "");
                        } else {
                            add_test_result("Sim_Amoeba Simulation", "MonteCarlo Optimization", "Optmization Failed");
                        }
                        trick_test_add_parent("Sim_Amoeba Simulation", "MonteCarlo Optimization", "1452306647 2014948908");
                        call_write_output();

                        exec_terminate( "cannon_pre_master",
                                        "Amoeba has found a solution." ) ;
                }

                if ( A->state == CALC_CENTROID_POINT ||
                     A->state == SHRINK ) {
                        continue ;
                } else {
                        break ;
                }
        }


        C->time_to_fire_jet_1 = A->curr_point[0] ;
        C->time_to_fire_jet_2 = A->curr_point[1] ;
        C->time_to_fire_jet_3 = A->curr_point[2] ;
        C->time_to_fire_jet_4 = A->curr_point[3] ;

        return(0) ;
}

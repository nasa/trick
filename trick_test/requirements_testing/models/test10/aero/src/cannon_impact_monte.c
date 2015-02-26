/*********************************** TRICK HEADER **************************
PURPOSE:                     (Kaboom!!!)
***************************************************************************/
#include <stdio.h>
#include "../include/cannon_aero.h"
#include "sim_services/include/executive.h" 
#include "sim_services/include/exec_proto.h"
#include "sim_services/include/regula_falsi.h"
#include "sim_services/include/dr_proto.h"

double cannon_impact_monte(
                            
  CANNON_AERO* C,                
  double* time,             
  int *event_evaluate_tgo ) 

{
        double tgo ;

        EXECUTIVE* E ; 
        E = exec_get_exec();

        if( *event_evaluate_tgo ) {
     
              /* Calculate time to go before impact */
               C->rf.error = C->pos[2] ;
               tgo = regula_falsi( *time , &(C->rf) ) ;
     
        } else {
     
               /*  Ball impact */

               reset_regula_falsi( *time , &(C->rf) ) ;
               tgo = 0.0 ;
           
               C->vel[0] = 0.0 ; C->vel[1] = 0.0 ; C->vel[2] = 0.0 ;
               C->acc[0] = 0.0 ; C->acc[1] = 0.0 ; C->acc[2] = 0.0 ;
               C->g = 0.0 ;

               fprintf(stderr, "Impact time, pos : %.9lf %.9lf\n", 
                        *time, C->pos[0] );

               dr_record_binary( &E->record.group[0], *time);
        }

        return( tgo ) ;
}

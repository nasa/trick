/*******************************************************************************
PURPOSE: (Malfunction trigger.)
*******************************************************************************/

#include <math.h>
#include "../include/test4.h"
#include "sim_services/include/exec_proto.h"

int trigger_job_1(TEST4* T) {
	double time = exec_get_sim_time();
        static int sign = 0; 
        int flip = 0;

	printf("%f: trigger_job_1() called: returned ", time);

        if (T->cos_pi_t >= 0.0 ) {
           if (sign < 0 ) {
               flip = 1;
           }
           sign = 1;
        } else {
           if (sign > 0 ) {
               flip = 1; 
           }
           sign = -1; 
        }
        
	if ( flip ) {
		printf("true\n");
		return 1;
	}
	printf("false\n");
	return 0;
}

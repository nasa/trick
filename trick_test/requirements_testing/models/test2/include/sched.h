/********************************* TRICK HEADER *******************************
PURPOSE: ( Tests requirements: 3.1.2.3, 3.1.2.1.1.1-7, 3.1.2.1.2.1-6,
			       3.1.2.1.3.1-5, 3.3.2.1.11 ) 
*******************************************************************************/

#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "sim_services/include/regula_falsi.h"

typedef struct {
  double pos;           /* M      Position */
  double vel;           /* M/s    Velocity */ 
  double acc;           /* M/s2   Acceleration */
  REGULA_FALSI rf ;     /* -- Dynamic event */
} MSD_NUMERIC ;

typedef struct {
  double pos;           /* M      Position */
  double vel;           /* M/s    Velocity */
  double omega_n;
  double omega_d;
  double zeta;
  double C1;
  double C2;
} MSD_ANALYTIC ;

typedef struct {
  double mass;          /* kg     Mass */
  double damping_const; /* kg/s   Damping constant */
  double spring_const;  /* kg/s2  Spring constant */
  double initial_pos;   /* M      Position */
  double initial_vel;   /* M/s    Velocity */
} MSD_PARAMS ;

typedef struct {
  MSD_PARAMS   params;
  MSD_NUMERIC  numeric;
  MSD_ANALYTIC analytic;
  double       delta_pos; /* M      Delta Position */
  double       delta_vel; /* M/s    Delta Velocity */
} MSD;

typedef struct { /* SCHEDULE ------------------------------------------------*/

  MSD msd;              
  int async_completion_count ; /* --     the number of times sched_async() has run to completion. */
  int amf_completion_count ;   /* --     the number of times sched_amf() has run to completion. */

} SCHEDULE ; /*--------------------------------------------------------------*/

#endif

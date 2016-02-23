/************************TRICK HEADER*************************
 * PURPOSE:
      (This comment lists out the other object files that are not included from c++ headers)
      LIBRARY DEPENDENCIES:
          (
               (sched/src/sched_amf.o)
               (sched/src/sched_async.o)
               (sched/src/sched_deriv.o)
               (sched/src/sched_dyn_event.o)
               (sched/src/sched_effector.o)
               (sched/src/sched_effector_emitter.o)
               (sched/src/sched_effector_receiver.o)
               (sched/src/sched_environment.o)
               (sched/src/sched_init.o)
               (sched/src/sched_integ.o)
               (sched/src/sched_logging.o)
               (sched/src/sched_scheduled.o)
               (sched/src/sched_sensor.o)
               (sched/src/sched_sensor_emitter.o)
               (sched/src/sched_sensor_receiver.o)
               (sched/src/sched_sensor_reflector.o)
               (sched/src/sched_default_data.o)
                                                     )
*************************************************************/

#ifndef SCHED_PROTO_H
#define SCHED_PROTO_H
#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

int sched_init(SCHEDULE*) ;
int sched_integ(SCHEDULE*) ;
int sched_deriv(SCHEDULE*) ;
double sched_dyn_event(SCHEDULE*) ;
int sched_scheduled(SCHEDULE*) ;
int sched_environment(SCHEDULE*) ;
int sched_sensor(SCHEDULE*) ;
int sched_sensor_reflector(SCHEDULE*) ;
int sched_sensor_receiver(SCHEDULE*) ;
int sched_sensor_emitter(SCHEDULE*) ;
int sched_effector_receiver(SCHEDULE*) ;
int sched_effector_emitter(SCHEDULE*) ;
int sched_effector(SCHEDULE*) ;
int sched_logging(SCHEDULE*) ;
int sched_amf(SCHEDULE*) ;
int sched_async(SCHEDULE*) ;
int sched_default_data(SCHEDULE*) ;

#ifdef __cplusplus
}
#endif

#endif


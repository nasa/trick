/********************************* TRICK HEADER *******************************
PURPOSE:
   (Simple taget body tagetting information.)
REFERENCES:
   ((None))
ASSUMPTIONS AND LIMITATIONS:
   ((Purely Keplerian dynamics.))
PROGRAMMERS:
   (((Edwin Z. Crues) (NASA) (Nov 2009) (--) (Initial version.)))
*******************************************************************************/

#ifndef _target_proto_h_
#define _target_proto_h_
#include "target_body.h"
#include "trick_utils/comm/include/tc_proto.h"

#ifdef __cplusplus
extern "C" {
#endif

int target_data_default_data(TargetBodyData*);
int target_delta_v(TargetBodyState*, TargetBodyData*, TargetBodyIteration*);
int target_earth_default_data(TargetBodyPlanet*);
void target_eom(TargetBodyPlanet*, TargetBodyState*, TargetBodyData*);
int target_integ(TargetBodyState*, TargetBodyData*);
int target_iterate_default_data(TargetBodyIteration*);
int target_master_init(TargetBodyIteration*);
int target_master_post(TCDevice*, TargetBodyInit*, TargetBodyState*, TargetBodyData*, TargetBodyIteration*);
int target_master_pre(TargetBodyInit*, TargetBodyState*, TargetBodyData*, TargetBodyIteration*);
int target_master_shutdown(TCDevice*, TargetBodyData*, TargetBodyIteration*);
void target_print(double, TargetBodyInit*, TargetBodyState*, TargetBodyIteration*);
int target_slave_init(TargetBodyIteration*);
int target_slave_post(TCDevice*, TargetBodyInit*, TargetBodyState*, TargetBodyData*, TargetBodyIteration*);
int target_slave_pre(TargetBodyInit*, TargetBodyState*, TargetBodyData*, TargetBodyIteration*);
int target_slave_shutdown(TCDevice*, TargetBodyData*);
void target_state_init(TargetBodyInit*, TargetBodyPlanet*, TargetBodyState*);

#ifdef __cplusplus
}
#endif

#endif 


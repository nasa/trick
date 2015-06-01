/*
    PURPOSE:
        (Trick runtime simulation executive parameter definition.)
    ICG: (No)
    REFERENCE:
        (((Bailey, R.W, and Paddock, E.J.) (Trick Simulation Environment)
          (NASA:JSC #37943)
          (JSC / Engineering Directorate / Automation and Robotics Division)
          (June 1994) (--)))
    ASSUMPTIONS AND LIMITATIONS:
        ((Only 64 levels of nested input data file inclusion.))
    PROGRAMMERS:
        (((Robert W. Bailey) (LinCom) (4/92) (--) (Realtime))
         ((Robert W. Bailey) (LinCom) (6/1/91) (Trick-CR-00000) (Initial Release)))
*/

#ifndef MESSAGE_PROTO_HH
#define MESSAGE_PROTO_HH

#include "trick/MessageSubscriber.hh"

int message_subscribe( Trick::MessageSubscriber * in_ms ) ;
int message_unsubscribe( Trick::MessageSubscriber * in_ms ) ;

#endif


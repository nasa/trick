/*
PURPOSE:
    (Macros useful for collect mechanism of S_define)
PROGRAMMER:
    (((Robert W. Bailey) (LinCom) (7/95) (--) (Realtime)))
*/

#include "trick/exec_proto.h"

#ifndef COLLECT_MACROS_H
#define COLLECT_MACROS_H

#define NUM_COLLECT(X) (( X == 0 ) ? 0 : *((long *)X - 1))

#endif

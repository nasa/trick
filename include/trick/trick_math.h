#ifndef TRICK_MATH_H
#define TRICK_MATH_H

/*
 * Function prototypes for all functions in ${TRICK_HOME}/sim_services/math
 */

#include <math.h>

#ifndef M_ABS
#define M_ABS(x) ((x) < 0 ? -(x) : (x))
#endif

#if ( __ghs )
#ifndef M_PI
#define M_PI     3.141592653589793238460
#endif

#ifndef M_PI_2
#define M_PI_2   1.570796326794896619231
#endif
#endif

#include "vector_macros.h"
#include "matrix_macros.h"
#include "quat_macros.h"
#include "trick_math_error.h"
#include "trick_math_proto.h"

#endif

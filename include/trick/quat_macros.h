/*
PURPOSE:
    (Quaternion math in line code macros.)

REFERENCE:
    ((None))

PROGRAMMERS:
    (((David Hammen) (Odyssey) (May 2005) (RDLaa07016) (Initial release.)))
*/

/**
@page QUATERNIAN_MACROS Quaternian Macros

Include:
$TRICK_HOME/trick_source/trick_utils/math/include/quat_macros.h

This set of macros operates on quaternians which are represented
as an array of four floating point types (i,e., double quat[4] or float quat[4]).

- Element 0 of the array represents the scalar part of the quaternian.
- Elements 1..3 of the array represent the vector part of the quaternian.
*/

#ifndef _QUAT_MACROS_H_
#define _QUAT_MACROS_H_

#include "trick/vector_macros.h"

/**
@page QUATERNIAN_MACROS
\b Q_IDENT( Q)

Set Q to the identity quaternian.
Q = (1.0, 0.0, 0.0, 0.0)
*/
#define Q_IDENT(quat) { \
   *quat = 1.0; \
   V_INIT ((quat+1)); \
}

/**
@page QUATERNIAN_MACROS
\b Q_COPY(D, S)

Copy quaternian S to quaternian D.
*/
#define Q_COPY(dest,src) { \
   *dest = *src; \
   V_COPY ((dest+1), (src+1)); \
}

/**
@page QUATERNIAN_MACROS
\b Q_SCALE(D, Q, C)

Assign the product of quaternian Q and scalar C to quaternian D.
*/
#define Q_SCALE(dest,src,scale) { \
   *dest = *src * scale; \
   V_SCALE ((dest+1), (src+1), scale); \
}

/**
@page QUATERNIAN_MACROS
\b Q_TRANS(D, Q)

Assign the conjugate of quaternian Q to quaternian D.
*/
#define Q_TRANS(dest,src) { \
   *dest = *src; \
   V_NEGATE ((dest+1), (src+1)); \
}

/**
@page QUATERNIAN_MACROS
\b QxQ(D, A, B)

Assign the product of quaternian A and quaternian B to quaternian D.
*/
#define QxQ(dest,quat1,quat2) { \
   *dest = *quat1 * *quat2 - V_DOT ((quat1+1), (quat2+1)); \
   V_SCALE ((dest+1), (quat2+1), *quat1); \
   VxS_ADD ((dest+1), (quat1+1), *quat2); \
   VxV_ADD ((dest+1), (quat1+1), (quat2+1)); \
}

/**
@page QUATERNIAN_MACROS
\b QtxQ(D, A, B)

Assign the product of quaternian A conjugate and B to quaternian D.
*/
#define QtxQ(dest,quat1,quat2) { \
   *dest = *quat1 * *quat2 + V_DOT ((quat1+1), (quat2+1)); \
   V_SCALE ((dest+1), (quat2+1), *quat1); \
   VxS_SUB ((dest+1), (quat1+1), *quat2); \
   VxV_SUB ((dest+1), (quat1+1), (quat2+1)); \
}

/**
@page QUATERNIAN_MACROS
\b QxQt(D, A, B)

Assign the product of quaternian A and quaternian B conjugate to quaternian D.
*/
#define QxQt(dest,quat1,quat2) { \
   *dest = *quat1 * *quat2 + V_DOT ((quat1+1), (quat2+1)); \
   V_SCALE ((dest+1), (quat1+1), *quat2); \
   VxS_SUB ((dest+1), (quat2+1), *quat1); \
   VxV_SUB ((dest+1), (quat1+1), (quat2+1)); \
}

/**
@page QUATERNIAN_MACROS
\b VxQ(D, V, Q)

Assign the product of vector V and quaternian Q to quaternian D.
*/
#define VxQ(dest,vec,quat) { \
   *dest = - V_DOT (vec, (quat+1)); \
   V_SCALE ((dest+1), vec, *quat); \
   VxV_ADD ((dest+1), vec, (quat+1)); \
}

/**
@page QUATERNIAN_MACROS
\b QxV(D, Q, V)

Assign the product of quaternian Q and vector V to quaternian D.
*/
#define QxV(dest,quat,vec) { \
   *dest = - V_DOT ((quat+1), vec); \
   V_SCALE ((dest+1), vec, *quat); \
   VxV_ADD ((dest+1), (quat+1), vec); \
}

/**
@page QUATERNIAN_MACROS
\b Q_dot(Qdot, Q, V)

Assign the time derivative of quaternian Q given an angular velocity V to Qdot.
*/
#define Q_dot(qdot,quat,omega) { \
   double v[3];\
   V_SCALE(v, omega , -0.5);\
   *qdot = - V_DOT((quat+1), v);\
   V_SCALE ((qdot+1), v,  *quat);\
   VxV_ADD ((qdot+1), v, (quat+1)); \
}

#endif /* _QUAT_MACROS_H_   DO NOT PUT ANYTHING AFTER THIS LINE */

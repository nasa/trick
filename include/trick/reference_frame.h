/*
PURPOSE:
    (Reference frame definition parameter definition)

ASSUMPTIONS AND LIMITATIONS:
    ((Must use three mutualy exculsive angles in Euler rotation sequence))

PROGRAMMERS:
    (((Robert W. Bailey) (LinCom Corp) (10/1/91) (--) (--)))
*/

#ifndef REFERENCE_FRAME_H
#define REFERENCE_FRAME_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {

    Roll_Pitch_Yaw, /* Roll Pitch Yaw sequence */
    Roll_Yaw_Pitch, /* Roll Yaw Pitch sequence */
    Pitch_Yaw_Roll, /* Pitch Yaw Roll sequence */
    Pitch_Roll_Yaw, /* Pitch Roll Yaw sequence */
    Yaw_Roll_Pitch, /* Yaw Roll Pitch sequence */
    Yaw_Pitch_Roll  /* Yaw Pitch Roll sequence */
} Euler_Seq;

typedef struct {

    Euler_Seq euler_sequence;   /* --   Euler rotation sequence */
    double euler_angles[3];     /* (rad)  Euler rot angles from
                                        Frame 1 to Frame 2 */
    double location[3];         /* (m)  Frame 1 origin to Frame 2
                                        origin vector, referenced
                                        to Frame 1 */
} TRANSFORM;

#ifdef __cplusplus
}
#endif
#endif

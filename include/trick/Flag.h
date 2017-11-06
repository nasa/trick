/*
PURPOSE:
    (Flag enumerated typedef definition.)
*/

#ifndef FLAG_H
#define FLAG_H

/*=== Protect against 'TRUE' and 'FALSE' already being '#define'd ===*/
#ifdef TRUE
#undef TRUE
#define TRUE_RESET
#endif

#ifdef FALSE
#undef FALSE
#define FALSE_RESET
#endif

#ifdef True
#undef True
#define True_RESET
#endif

#ifdef False
#undef False
#define False_RESET
#endif

#ifdef __cplusplus
extern "C" {
#endif

        typedef enum {

                Off = 0,
                On = 1,
                No = 0,
                Yes = 1,
#ifndef SWIG
                False = 0,
                True = 1,
#endif
                Disconnect = 0,
                Connect = 1,
                Inactive = 0,
                Active = 1,
                Stop = 0,
                Go = 1,
                Low = 0,
                High = 1,
                Disable = 0,
                Enable = 1,
                TRUE = 255,     /* FORTRAN Logical TRUE */
                FALSE = 0,      /* FORTRAN Logical FALSE */
                Open = 0,
                Closed = 1,

                /* Stamps flags */
                OFF = 0,
                ON = 1,
                NO = 0,
                YES = 1,
                INACTIVE = 0,
                ACTIVE = 1,
                BACKWARD = 0,
                FORWARD = 1,
                BAD = 0,
                GOOD = 1
        } Flag;

#ifdef __cplusplus
}
#endif
typedef Flag LOGICAL_TYPE;

#ifdef TRUE_RESET
#undef TRUE_RESET
#define TRUE 1
#endif
#ifdef FALSE_RESET
#undef FALSE_RESET
#define FALSE 0
#endif

#ifdef True_RESET
#undef True_RESET
#define True 1
#endif
#ifdef False_RESET
#undef False_RESET
#define False 0
#endif

#endif

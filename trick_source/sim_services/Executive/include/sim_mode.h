/*
PURPOSE: 
    (For master/slave sim, this is slave info)
PROGRAMMERS:
    (((Eddie J. Paddock) (LinCom) (April 1996) (--) (Initial Version))
     ((Scott Killingsworth) (LinCom) (September 1997) (--) (--)))
*/

/*
 * $Id: sim_mode.h 3046 2013-06-19 18:30:47Z alin $
 */

/**
 * @class SIM_COMMAND sim_mode.h
 * The SIM_COMMAND enumeration represents the TRICK simulation commmands.
 */

#ifndef _SIMMODE_HH_
#define _SIMMODE_HH_

typedef enum {

    NoCmd = 0 ,         /* NoCmd */
    FreezeCmd = 2 ,     /* freeze */
    RunCmd = 3 ,        /* run */
    ExitCmd = 10 ,      /* exit */

} SIM_COMMAND;

/**
 * @class SIM_MODE sim_mode.h
 * The SIM_MODE enumeration represents the TRICK simulation modes.
 */

typedef enum {

    Initialization = 0 ,  /* Initialization */
    Freeze = 1 ,          /* freeze */
    Step = 4 ,            /* Debug Stepping */
    Run = 5 ,             /* run */
    ExitMode = 6          /* exit */

} SIM_MODE ;

#endif

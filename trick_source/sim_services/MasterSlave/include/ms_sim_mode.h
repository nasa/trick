/*
PURPOSE:
    (For master/slave sim, this is slave info)
PROGRAMMERS:
    (((Eddie J. Paddock) (LinCom) (April 1996) (--) (Initial Version))
     ((Scott Killingsworth) (LinCom) (September 1997) (--) (--)))
*/

/**
 * @class MS_SIM_COMMAND sim_mode.h
 * The MS_SIM_COMMAND enumeration represents the TRICK simulation commmands.
 */

#ifndef MS_SIMMODE_HH
#define MS_SIMMODE_HH

#define MS_ERROR_TIME -1000000000
#define MS_ERROR_PORT -1
#define MS_ERROR_NAME -1

typedef enum {

    MS_NoCmd =               0,  /* Executive mode: NoCmd */
    MS_FreezeCmd =           2,  /* Executive mode: freeze */
    MS_RunCmd =              3,  /* Executive mode: run */
    MS_ExitCmd =            10,  /* Executive mode: exit */
    MS_ErrorCmd =           11,  /* Master/Slave: an error was detected */
    MS_ReconnectCmd =       12,  /* Master/Slave: slave reconnected after chkpnt load */
    MS_ChkpntDumpAsciiCmd = 13,  /* Master/Slave: master tells slave to dump an ascii chkpnt */
    MS_ChkpntLoadAsciiCmd = 14,  /* Master/Slave: master tells slave to load an ascii chkpnt */
    MS_ChkpntDumpBinCmd =   15,  /* Master/Slave: master tells slave to dump a binary chkpnt */
    MS_ChkpntLoadBinCmd =   16   /* Master/Slave: master tells slave to load a binary chkpnt */

} MS_SIM_COMMAND;

#endif

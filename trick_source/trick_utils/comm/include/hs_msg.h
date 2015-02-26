/******************************************************************************
*                                                                             *
* Trick Simulation Environment Software                                       *
*                                                                             *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                     *
* All rights reserved.                                                        *
*                                                                             *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized   *
* use of Trick Software including source code, object code or executables is  *
* strictly prohibited and LinCom assumes no liability for such actions or     *
* results thereof.                                                            *
*                                                                             *
* Trick Software has been developed under NASA Government Contracts and       *
* access to it may be granted for Government work by the following contact:   *
*                                                                             *
* Contact: Charles Gott, Branch Chief                                         *
*          Flight Robotic Systems Branch                                      *
*          Automation, Robotics, & Simulation Division                        *
*          NASA, Johnson Space Center, Houston, TX                            *
*                                                                             *
******************************************************************************/

/*
 * $Id: hs_msg.h 49 2009-02-02 22:37:59Z lin $
 */

#define MAX_MSG_SIZE  4096

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {

    char sim_name[48];    /* -- Sim/Task-group name msg is 
                                coming from */
    char hostname[48];    /* -- Machine name msg is coming from */
    double ret;           /* -- Run elapsed time */
    int parent_child_num; /* -- Parent or child process number */
    int msgsize;          /* -- Size in bytes of following msg */
    char datebuff[32];    /* -- Date and time string */

    /* NOTE: msg[MAX_MSG_SIZE] MUST be the LAST 
     * element of this structure 
     */
    char msg[MAX_MSG_SIZE]; /* -- the message */
} HS_MSG;

#ifdef __cplusplus
}
#endif

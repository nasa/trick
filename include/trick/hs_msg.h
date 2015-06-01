
#ifndef HS_MSG_H
#define HS_MSG_H

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

#endif

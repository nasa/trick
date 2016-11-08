/*
    PURPOSE:
        (DMTCP C interface)
*/

#ifndef DMTCP_CHECKPOINT_C_INTF_HH
#define DMTCP_CHECKPOINT_C_INTF_HH

#ifdef _DMTCP
#include "dmtcpaware.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* call DMTCP function dmtcpIsEnabled() */
int dmtcp_is_enabled() ;

/* call DMTCP funcion dmtcpRunCommand() */
int call_dmtcp();

/* DMTCP jobs to run pre, post, and restart. Arguments for dmtcpInstallHooks() - defined in dmtcp/src/dmtcpaware.c */
void dmtcp_pre_checkpoint() ;
void dmtcp_post_checkpoint() ;
void dmtcp_restart() ;

/* set DMTCP checkpoint flag */
void dmtcp_set_checkpoint_now() ;

/* queue to store dmtcp job names */
void dmtcp_job_queue( std::string file_name );

#ifdef __cplusplus
}
#endif

#endif

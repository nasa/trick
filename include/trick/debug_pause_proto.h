
#ifndef DEBUGPAUSE_PROTO_H
#define DEBUGPAUSE_PROTO_H

#ifdef __cplusplus
extern "C" {
#endif

int debug_pause_on(void) ;
int debug_pause_off(void) ;
int debug_signal(void) ;

#ifdef __cplusplus
}
#endif

#endif

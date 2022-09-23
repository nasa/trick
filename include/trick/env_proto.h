
#ifndef ENV_PROTO_H
#define ENV_PROTO_H

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int env_add_var( const char * variable_name, const char * value) ;
const char * get_trick_env( char * variable_name ) ;
const char * env_get_var( const char * variable_name ) ;

int env_print_vars(FILE * fp) ;

#ifdef __cplusplus
}
#endif

#endif

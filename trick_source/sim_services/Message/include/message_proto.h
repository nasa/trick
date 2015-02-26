/*
 * $Id: message_proto.h 3530 2014-05-15 16:13:05Z alin $
 */

#ifndef _MESSAGE_PROTO_H_
#define _MESSAGE_PROTO_H_

#include <stdio.h>
#include <stdarg.h>
#include "message_type.h"

#ifdef __cplusplus
extern "C" {
#endif

int message_publish(int level, const char *format_msg, ...) ;
int message_publish_standalone(int level, const char *format_msg, ...) ;
int send_hs(FILE * fp, const char *format_msg, ...) ;

#ifndef SWIG
int vmessage_publish(int level, const char *format_msg, va_list args) ;
int vsend_hs(FILE * fp, const char *format_msg, va_list args) ;
#endif

#ifdef __cplusplus
}
#endif

#endif


#ifndef MESSAGE_PROTO_H
#define MESSAGE_PROTO_H

#include <stdio.h>
#include <stdarg.h>
#include "trick/message_type.h"

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


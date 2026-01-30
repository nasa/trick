#ifndef MESSAGE_PROTO_H
#define MESSAGE_PROTO_H

#include "trick/message_type.h"
#include <stdarg.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C"
{
#endif

    int message_add_subscriber(void * in_ms);
    int message_remove_subscriber(void * in_ms);
    void * message_get_subscriber(const char * sub_name);
    int message_publish(int level, const char * format_msg, ...);
    int message_publish_standalone(int level, const char * format_msg, ...);
    int send_hs(FILE * fp, const char * format_msg, ...);

#ifndef SWIG
    int vmessage_publish(int level, const char * format_msg, va_list args);
    int vsend_hs(FILE * fp, const char * format_msg, va_list args);
#endif

#ifdef __cplusplus
}
#endif

#endif

/*
 * $Id: message_proto.h 1635 2011-05-23 14:39:16Z dstrauss $
 */

#ifndef _MESSAGE_TYPE_H_
#define _MESSAGE_TYPE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @class Trick::MessagePublisher::MESSAGE_TYPE message_proto.h
 * The MESSAGE_TYPE enumeration represents types of messages to publish, which corresponds to the displayed color of the message.
 */
    typedef enum {
        MSG_NORMAL               =   0, /* default color */
        MSG_INFO                 =   1, /* green */
        MSG_WARNING              =   2, /* yellow */
        MSG_ERROR                =   3, /* red */
        MSG_DEBUG                =  10, /* cyan */
        MSG_PLAYBACK             =  101 /* variable server playback message */
    } MESSAGE_TYPE ;

#ifdef __cplusplus
}
#endif

#endif


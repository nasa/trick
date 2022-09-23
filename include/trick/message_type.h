#ifndef MESSAGE_TYPE_H
#define MESSAGE_TYPE_H
/*
    PURPOSE: ( Define message types for the message publisher. )
*/
#ifdef __cplusplus
extern "C" {
#endif

/**
 * @enum Trick::MessagePublisher::MESSAGE_TYPE
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


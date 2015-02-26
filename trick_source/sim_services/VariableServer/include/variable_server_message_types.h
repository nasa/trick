/*
    PURPOSE:
        (Enumeration of the Variable Message types)
*/

/*
 * $Id:$
 */

#ifndef _VARIABLESERVERMESSAGETYPES_HH_
#define _VARIABLESERVERMESSAGETYPES_HH_


typedef enum {
    VS_IP_ERROR = -1,
    VS_VAR_LIST = 0,
    VS_VAR_EXISTS = 1,
    VS_SIE_RESOURCE = 2,
    VS_LIST_SIZE = 3 ,
    VS_STDIO = 4
} VS_MESSAGE_TYPE ;

#endif


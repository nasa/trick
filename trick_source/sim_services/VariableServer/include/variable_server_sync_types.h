/*
    PURPOSE:
        (Enumeration of the Variable Message types)
*/

/*
 * $Id:$
 */

#ifndef _VARIABLESERVERSYNCTYPES_HH_
#define _VARIABLESERVERSYNCTYPES_HH_

typedef enum {
    VS_COPY_ASYNC = 0,
    VS_COPY_SCHEDULED = 1,
    VS_COPY_TOP_OF_FRAME = 2
} VS_COPY_MODE ;

typedef enum {
    VS_WRITE_ASYNC = 0,
    VS_WRITE_WHEN_COPIED = 1
} VS_WRITE_MODE ;

#endif


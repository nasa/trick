/*
PURPOSE: (Trick error handler function type definition)

PROGRAMMERS:
     (((Edwin Z. Crues) (LinCom) (29 February 2000) (--) (Generalized))
      ((Jane Falgout)(Titan Corp)(?)(--)(brought into trick))
      ((John Bullock)(Titan Corp)(July 2002)(--)
       (Changed error level enumeration)))
*/

#ifndef TRICK_ERROR_HNDLR_H
#define TRICK_ERROR_HNDLR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>


/* Defined error levels. */
#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    TRICK_ERROR_ALL = 0,      /* -- No error: Report everything. */
    TRICK_ERROR_TRIVIAL = 1,  /* -- Trivial level:
                                    Informational: can ignore  */
    TRICK_ERROR_ADVISORY = 2, /* -- Advisory level:
                                    Can probably ignore these  */

    TRICK_ERROR_CAUTION = 3,  /* -- Caution level:
                                    May not want to ignore these */
    TRICK_ERROR_WARNING = 4,  /* -- Warning level:
                                    Should probably not ignore */
    TRICK_ERROR_ALERT = 5,    /* -- Alert level:
                                    Should be corrected,
                                    may not recover */
    TRICK_ERROR_FATAL = 6,  /* -- Fatal level:
                                  Must be corrected, no recovery */
    TRICK_ERROR_ABORT = 7,  /* -- Abort level:
                                  Things are too bad, just abort */
    TRICK_ERROR_SILENT = 8  /* -- Silent: Do not report
                                      any error of any kind */
} TrickErrorLevel;


/* Declare untyped error data type. */
typedef void *TrickErrorDataPtr;

/* Forward declaration of error handler data type. */
struct _TrickErrorHndlr;

/* Define the error handler function type. */
typedef void (*TrickErrorFuncPtr) (struct _TrickErrorHndlr * this_error,
                                   TrickErrorLevel level,
                                   const char *file, int line, const char *msg);

/* Primary error handler data structure. */
typedef struct _TrickErrorHndlr {

    TrickErrorFuncPtr error_func;   /* ** Err handling function */
    TrickErrorDataPtr data_ptr;     /* ** Err handling data     */
    TrickErrorLevel report_level;   /* -- Err reporting level   */
    int use_send_hs[9];             /* -- 1=send_hs; else fprintf */
    FILE *report_stream[9];         /* -- Err reporting streams */

} TrickErrorHndlr;


/* Define error object access functions */
void trick_error_func_default(
        TrickErrorHndlr * error_hndlr, /* IN: -- Error object */
        TrickErrorLevel level,         /* IN: -- Error level */
        char *file,        /* IN: -- File in which error ocurred */
        int line,          /* IN: -- Line in which error ocurred */
        char *msg);        /* IN: -- Error message  */

void trick_error_init(  /* RETURN: -- None  */
        TrickErrorHndlr * error_hndlr,     /* IN: -- Error object */
        TrickErrorFuncPtr error_func,      /* IN: -- Error function */
        TrickErrorDataPtr error_data,      /* IN: -- Error data*/
        TrickErrorLevel report_level);     /* IN: -- Err report level */

void trick_error_shutdown(
        TrickErrorHndlr * error_hndlr);    /* IN: -- Error object */

void trick_error_set_func(
        TrickErrorHndlr * error_hndlr, /* INOUT: -- Err handler data */
        TrickErrorFuncPtr error_func); /* IN: -- Error function */

void trick_error_set_data(
        TrickErrorHndlr * error_hndlr, /* INOUT: -- Err handler data */
        TrickErrorDataPtr error_data); /* IN: -- Error data */

void trick_error_set_report_level(
        TrickErrorHndlr * error_hndlr, /* INOUT: -- Err handler data */
        TrickErrorLevel report_level); /* IN: -- Err reporting level */

void trick_error_set_stream(
        TrickErrorHndlr * error_hndlr, /* INOUT: -- Err handler data */
        TrickErrorLevel level,         /* IN: -- Err reporting level
                                                 stream to set. */
        FILE * stream);                /* IN: -- Err stream for
                                                 reporting level. */

void trick_error_set_all_streams(
        TrickErrorHndlr * error_hndlr,  /* INOUT: -- Err handler data */
        FILE * stream);                 /* IN: -- Error stream */

void trick_error_copy_streams(
        TrickErrorHndlr * dest_error_hndlr,  /* OUT: -- Err hdler data*/
        TrickErrorHndlr * src_error_hndlr);  /* IN:  -- Err hdler data*/

void trick_error_set_send_hs_flag(
        TrickErrorHndlr * error_hndlr, /* INOUT: -- Err handler data */
        TrickErrorLevel level,         /* IN: -- Error reporting level
                                                 stream to set. */
        int use_send_hs);              /* IN: -- 1=use send_hs;
                                                 else use fprintf */

void trick_error_set_all_send_hs_flags(
        TrickErrorHndlr * error_hndlr,  /* INOUT: -- Err handler data */
        int use_send_hs);               /* IN: -- 1= use send_hs;
                                                  else use fprintf */

void trick_error_copy_send_hs_flags(
        TrickErrorHndlr * dest_error_hndlr, /* OUT: -- Err hdler data */
        TrickErrorHndlr * src_error_hndlr); /* IN:  -- Err hdler data */

TrickErrorFuncPtr trick_error_get_func(
        TrickErrorHndlr * error_hndlr); /* INOUT: -- Err hdler data */

TrickErrorDataPtr trick_error_get_data(
        TrickErrorHndlr * error_hndlr); /* INOUT: -- Error hdler data */

TrickErrorLevel trick_error_get_report_level(
        TrickErrorHndlr * error_hndlr); /* INOUT: -- Error hdler data */

FILE *trick_error_get_stream(
        TrickErrorHndlr * error_hndlr,  /* INOUT: -- Error hdler data */
        TrickErrorLevel level);         /* IN: -- Error reporting
                                                  level stream to get */

FILE *trick_error_get_curr_stream(
        TrickErrorHndlr * error_hndlr); /* INOUT: -- Err hdler data */

int trick_error_get_send_hs_flag(
        TrickErrorHndlr * error_hndlr,  /* INOUT: -- Err hdler data */
        TrickErrorLevel level);         /* IN: -- Err report level
                                                  stream to get. */

int trick_error_get_curr_send_hs_flag(
        TrickErrorHndlr * error_hndlr); /* INOUT: -- Err hdler data */

void trick_error_report(
        TrickErrorHndlr * error_hndlr, /* IN: -- Error object */
        TrickErrorLevel error_level,   /* IN: -- Err level for
                                                 this error */
        const char *file,              /* IN: -- File in which error_ occured */
        int line,                      /* IN: -- Line number where error_ occured */
        const char *format,            /* IN: -- Error_ message format */
        ...);                          /* IN: ** variable length arg list */

#ifdef __cplusplus
}
#endif
#endif

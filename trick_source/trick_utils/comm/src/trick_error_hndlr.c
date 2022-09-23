/*
   PURPOSE: (Check for data which is configured in the input files but was not mapped to any imports or exports from
   the S_define file.)

   REFERENCE: ((None))

   ASSUMPTIONS AND LIMITATIONS: ((None))

   CLASS: (N/A)

   LIBRARY DEPENDENCY: ((trick_error_hndlr.o))

   PROGRAMMERS: (((Edwin Z. Crues) (LinCom Corp) (April 2000) (--) (created))
    ((Jane Falgout)(Titan Corp)(?)(--)(brought into trick))
    ((John Bullock)(Titan Corp)(July 2002)(--)(Changed error level enumeration))) */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#ifdef __WIN32__
#include <io.h>
#else
#include <unistd.h>
#endif

#include "trick/trick_error_hndlr.h"
//#include "trick/tc.h"

static TrickErrorHndlr trick_error_hndlr_default = {
    (TrickErrorFuncPtr) trick_error_func_default,
    (TrickErrorDataPtr *) NULL,
    TRICK_ERROR_ADVISORY,
    {0, 0, 0, 0, 0, 0, 0, 0, 0},
    {(FILE *) NULL, (FILE *) NULL, (FILE *) NULL, (FILE *) NULL,
     (FILE *) NULL, (FILE *) NULL, (FILE *) NULL, (FILE *) NULL,
     (FILE *) NULL}
};

static void trick_error_message_print(FILE * out_stream,        /* Inout: Output stream on which to print.  */
                                      TrickErrorLevel level,    /* In: Error level.  */
                                      char *file,       /* In: File in which error ocurred.  */
                                      int line, /* In: Line in file in which error ocurred. */
                                      char *msg) {      /* In: Error message.  */

    char *label[] = { "ALL", "TRIVIAL", "ADVISORY", "CAUTION",
        "WARNING", "ALERT", "FATAL", "ABORT", "SILENT"
    };

    /* Print message to the output stream. */
    if (msg != (char *) NULL) {
        if (file != (char *) NULL) {
            if (line > 0) {
                fprintf(out_stream, "\n %s: %s:%d: %s\n", label[level], file, line, msg);
            } else {
                fprintf(out_stream, "\n %s: %s: %s\n", label[level], msg, file);
            }
        } else {
            fprintf(out_stream, "\n %s: %s", label[level], msg);
        }
    } else {
        if (file != (char *) NULL) {
            if (line > 0) {
                fprintf(out_stream, "\n %s: found near line %d of " "file: %s\n", label[level], line, file);
            } else {
                fprintf(out_stream, "\n %s: found in file: %s\n", label[level], file);
            }
        } else {
            fprintf(out_stream, "\n %s: an undescribed error was " "reported.\n", label[level]);
        }
    }

    return;
}


void trick_error_func_default(TrickErrorHndlr * error_hndlr,    /* In: Error object */
                              TrickErrorLevel level,    /* In: Error level */
                              char *file,       /* In: File in which error ocurred */
                              int line, /* In: Line in file in which error ocurred */
                              char *msg)
{                                      /* In: Error message */
    char *msg_buf;
    char unknown[128];

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL) {
        error_hndlr = &(trick_error_hndlr_default);
    }

    /* Check for the error level of this error message. */
    switch (level) {

        case TRICK_ERROR_SILENT:
            /* This implies that both the reporting level and error level are set to TRICK_ERROR_SILENT; in practice,
               no error level should ever be set higher than TRICK_ERROR_ABORT.  Theoretically, nothing should ever
               be printed out when the reporting level is set to TRICK_ERROR_SILENT.  However, by passing in the error
               level as TRICK_ERROR_SILENT this implies that the user really wants to see this message. */
            if (error_hndlr->report_stream[level] == (FILE *) NULL) {
                error_hndlr->report_stream[level] = stderr;
            }
            trick_error_message_print(error_hndlr->report_stream[level], level, file, line, msg);
            break;

        case TRICK_ERROR_ABORT:
            if (error_hndlr->report_stream[level] == (FILE *) NULL) {
                error_hndlr->report_stream[level] = stderr;
            }
            trick_error_message_print(error_hndlr->report_stream[level], level, file, line, msg);
            abort();
            break;

        case TRICK_ERROR_FATAL:
            if (error_hndlr->report_stream[level] == (FILE *) NULL) {
                error_hndlr->report_stream[level] = stderr;
            }
            trick_error_message_print(error_hndlr->report_stream[level], level, file, line, msg);
            exit((int) level);
            break;

        case TRICK_ERROR_ALERT:
            if (error_hndlr->report_stream[level] == (FILE *) NULL) {
                error_hndlr->report_stream[level] = stderr;
            }
            trick_error_message_print(error_hndlr->report_stream[level], level, file, line, msg);
            break;

        case TRICK_ERROR_WARNING:
            if (error_hndlr->report_stream[level] == (FILE *) NULL) {
                error_hndlr->report_stream[level] = stderr;
            }
            trick_error_message_print(error_hndlr->report_stream[level], level, file, line, msg);
            break;

        case TRICK_ERROR_CAUTION:
            if (error_hndlr->report_stream[level] == (FILE *) NULL) {
                error_hndlr->report_stream[level] = stderr;
            }
            trick_error_message_print(error_hndlr->report_stream[level], level, file, line, msg);
            break;

        case TRICK_ERROR_ADVISORY:
            if (error_hndlr->report_stream[level] == (FILE *) NULL) {
                error_hndlr->report_stream[level] = stderr;
            }
            trick_error_message_print(error_hndlr->report_stream[level], level, file, line, msg);
            break;

        case TRICK_ERROR_TRIVIAL:
            if (error_hndlr->report_stream[level] == (FILE *) NULL) {
                error_hndlr->report_stream[level] = stderr;
            }
            trick_error_message_print(error_hndlr->report_stream[level], level, file, line, msg);
            break;

        case TRICK_ERROR_ALL:
            if (error_hndlr->report_stream[level] == (FILE *) NULL) {
                error_hndlr->report_stream[level] = stderr;
            }
            trick_error_message_print(error_hndlr->report_stream[level], level, file, line, msg);
            break;

        default:
            if (msg != (char *) NULL) {
                sprintf(unknown, "Unknown error level: %d!  Something is " "seriously wrong!\n   ", level);
                msg_buf = (char *) malloc(strlen(msg) + strlen(unknown) + 2);
                strcpy(msg_buf, unknown);
                strcat(msg_buf, msg);
                trick_error_message_print(stderr, TRICK_ERROR_ALERT, file, line, msg_buf);
                free(msg_buf);
            } else {
                trick_error_message_print(stderr, TRICK_ERROR_ALERT, file, line, unknown);
            }
            break;

    }

    return;

}


void trick_error_init(TrickErrorHndlr * error_hndlr,    /* In: Error object.  */
                      TrickErrorFuncPtr error_func,     /* In: Error function.  */
                      TrickErrorDataPtr error_data,     /* In: Error data.  */
                      TrickErrorLevel report_level)
{                                      /* In: Error reporting level. */

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL) {
        error_hndlr = &(trick_error_hndlr_default);
    }

    /* Initialize the error handling function. */
    if (error_func != (TrickErrorFuncPtr) NULL) {
        error_hndlr->error_func = error_func;
    } else {
        error_hndlr->error_func = (TrickErrorFuncPtr) trick_error_func_default;
    }

    /* Initialize the error data. */
    if (error_data != (TrickErrorDataPtr) NULL) {
        error_hndlr->data_ptr = error_data;
    } else {
        error_hndlr->data_ptr = (TrickErrorDataPtr) NULL;
    }

    /* Set the initial error reporting level. */
    error_hndlr->report_level = report_level;

    /* Set the default reporting streams. */
    error_hndlr->report_stream[TRICK_ERROR_ALL] = stderr;
    error_hndlr->report_stream[TRICK_ERROR_TRIVIAL] = stderr;
    error_hndlr->report_stream[TRICK_ERROR_ADVISORY] = stderr;
    error_hndlr->report_stream[TRICK_ERROR_CAUTION] = stderr;
    error_hndlr->report_stream[TRICK_ERROR_WARNING] = stderr;
    error_hndlr->report_stream[TRICK_ERROR_ALERT] = stderr;
    error_hndlr->report_stream[TRICK_ERROR_FATAL] = stderr;
    error_hndlr->report_stream[TRICK_ERROR_ABORT] = stderr;
    error_hndlr->report_stream[TRICK_ERROR_SILENT] = stderr;

    error_hndlr->use_send_hs[TRICK_ERROR_ALL] = 1;
    error_hndlr->use_send_hs[TRICK_ERROR_TRIVIAL] = 1;
    error_hndlr->use_send_hs[TRICK_ERROR_ADVISORY] = 1;
    error_hndlr->use_send_hs[TRICK_ERROR_CAUTION] = 1;
    error_hndlr->use_send_hs[TRICK_ERROR_WARNING] = 1;
    error_hndlr->use_send_hs[TRICK_ERROR_ALERT] = 1;
    error_hndlr->use_send_hs[TRICK_ERROR_FATAL] = 1;
    error_hndlr->use_send_hs[TRICK_ERROR_ABORT] = 1;
    error_hndlr->use_send_hs[TRICK_ERROR_SILENT] = 1;

    return;

}

void trick_error_shutdown(TrickErrorHndlr * error_hndlr)
{
    FILE *fptr;

    if (error_hndlr == (TrickErrorHndlr *) NULL)
        return;

    error_hndlr->error_func = (TrickErrorFuncPtr) NULL;
    error_hndlr->data_ptr = (TrickErrorDataPtr) NULL;
    error_hndlr->report_level = TRICK_ERROR_SILENT;

    fptr = error_hndlr->report_stream[TRICK_ERROR_ALL];
    if ((fptr != (FILE *) NULL) && (fptr != stderr) && (fptr != stdout))
        close(fileno(fptr));

    fptr = error_hndlr->report_stream[TRICK_ERROR_TRIVIAL];
    if ((fptr != (FILE *) NULL) && (fptr != stderr) && (fptr != stdout))
        close(fileno(fptr));

    fptr = error_hndlr->report_stream[TRICK_ERROR_ADVISORY];
    if ((fptr != (FILE *) NULL) && (fptr != stderr) && (fptr != stdout))
        close(fileno(fptr));

    fptr = error_hndlr->report_stream[TRICK_ERROR_CAUTION];
    if ((fptr != (FILE *) NULL) && (fptr != stderr) && (fptr != stdout))
        close(fileno(fptr));

    fptr = error_hndlr->report_stream[TRICK_ERROR_WARNING];
    if ((fptr != (FILE *) NULL) && (fptr != stderr) && (fptr != stdout))
        close(fileno(fptr));

    fptr = error_hndlr->report_stream[TRICK_ERROR_ALERT];
    if ((fptr != (FILE *) NULL) && (fptr != stderr) && (fptr != stdout))
        close(fileno(fptr));

    fptr = error_hndlr->report_stream[TRICK_ERROR_FATAL];
    if ((fptr != (FILE *) NULL) && (fptr != stderr) && (fptr != stdout))
        close(fileno(fptr));

    fptr = error_hndlr->report_stream[TRICK_ERROR_ABORT];
    if ((fptr != (FILE *) NULL) && (fptr != stderr) && (fptr != stdout))
        close(fileno(fptr));

    fptr = error_hndlr->report_stream[TRICK_ERROR_SILENT];
    if ((fptr != (FILE *) NULL) && (fptr != stderr) && (fptr != stdout))
        close(fileno(fptr));

    error_hndlr->report_stream[TRICK_ERROR_ALL] = (FILE *) NULL;
    error_hndlr->report_stream[TRICK_ERROR_TRIVIAL] = (FILE *) NULL;
    error_hndlr->report_stream[TRICK_ERROR_ADVISORY] = (FILE *) NULL;
    error_hndlr->report_stream[TRICK_ERROR_CAUTION] = (FILE *) NULL;
    error_hndlr->report_stream[TRICK_ERROR_WARNING] = (FILE *) NULL;
    error_hndlr->report_stream[TRICK_ERROR_ALERT] = (FILE *) NULL;
    error_hndlr->report_stream[TRICK_ERROR_FATAL] = (FILE *) NULL;
    error_hndlr->report_stream[TRICK_ERROR_ABORT] = (FILE *) NULL;
    error_hndlr->report_stream[TRICK_ERROR_SILENT] = (FILE *) NULL;

    return;

}


void trick_error_set_func(TrickErrorHndlr * error_hndlr,        /* Inout: Error handler data. */
                          TrickErrorFuncPtr error_func)
{                                      /* In: Error function.  */

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL) {
        error_hndlr = &(trick_error_hndlr_default);
    }

    /* Set the error function. */
    if (error_func != (TrickErrorFuncPtr) NULL) {
        error_hndlr->error_func = error_func;
    } else {
        error_hndlr->error_func = (TrickErrorFuncPtr) trick_error_func_default;
    }

    return;
}


void trick_error_set_data(TrickErrorHndlr * error_hndlr,        /* Inout: Error handler data. */
                          TrickErrorDataPtr error_data)
{                                      /* In: Error data.  */

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL) {
        error_hndlr = &(trick_error_hndlr_default);
    }

    /* Set the error data. */
    if (error_data != (TrickErrorDataPtr) NULL) {
        error_hndlr->data_ptr = error_data;
    } else {
        error_hndlr->data_ptr = (TrickErrorDataPtr) NULL;
    }

    return;
}


void trick_error_set_report_level(TrickErrorHndlr * error_hndlr,        /* Inout: Error handler data. */
                                  TrickErrorLevel report_level)
{                                      /* In: Error reporting level. */

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL) {
        error_hndlr = &(trick_error_hndlr_default);
    }

    /* Set the reporting level. */
    error_hndlr->report_level = report_level;

    return;
}


void trick_error_set_stream(TrickErrorHndlr * error_hndlr,      /* INOUT: Error handler data */
                            TrickErrorLevel level,      /* IN: Err report level stream to set */
                            FILE * stream)
{                                      /* IN: Err stream for report level */

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL) {
        error_hndlr = &(trick_error_hndlr_default);
    }

    /* Check sanity of reporting level. */
    if (level > TRICK_ERROR_SILENT) {
        trick_error_report(error_hndlr, TRICK_ERROR_ALERT,
                           __FILE__, __LINE__, "Unknown error level for " "trick_error_set_stream; level unchanged.\n");
        return;
    }

    /* Set the appropriate file stream pointer. */
    if (stream == (FILE *) NULL) {
        error_hndlr->report_stream[level] = stderr;
    } else {
        error_hndlr->report_stream[level] = stream;
    }
    return;

}


void trick_error_set_all_streams(TrickErrorHndlr * error_hndlr, /* Inout: Error handler data */
                                 FILE * stream)
{                                      /* In: Error stream */
    FILE *act_stream;

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL) {
        error_hndlr = &(trick_error_hndlr_default);
    }

    /* Set the appropriate file stream pointer. */
    if (stream == (FILE *) NULL) {
        act_stream = stderr;
    } else {
        act_stream = stream;
    }

    error_hndlr->report_stream[TRICK_ERROR_ALL] = act_stream;
    error_hndlr->report_stream[TRICK_ERROR_TRIVIAL] = act_stream;
    error_hndlr->report_stream[TRICK_ERROR_ADVISORY] = act_stream;
    error_hndlr->report_stream[TRICK_ERROR_CAUTION] = act_stream;
    error_hndlr->report_stream[TRICK_ERROR_WARNING] = act_stream;
    error_hndlr->report_stream[TRICK_ERROR_ALERT] = act_stream;
    error_hndlr->report_stream[TRICK_ERROR_FATAL] = act_stream;
    error_hndlr->report_stream[TRICK_ERROR_ABORT] = act_stream;
    error_hndlr->report_stream[TRICK_ERROR_SILENT] = act_stream;

    return;

}

void trick_error_copy_streams(TrickErrorHndlr * dest_error_hndlr,       /* Out: Error handler data */
                              TrickErrorHndlr * src_error_hndlr)
{                                      /* In: Error handler data */
    memcpy((void *) dest_error_hndlr->report_stream, (void *) src_error_hndlr->report_stream, (sizeof(FILE *) * 9));
}


void trick_error_set_send_hs_flag(TrickErrorHndlr * error_hndlr,        /* Inout: Err handler data */
                                  TrickErrorLevel level,        /* In: Err report level stream to set */
                                  int use_send_hs)
{                                      /* In: 1= use send_hs;else use fprintf */

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL) {
        error_hndlr = &(trick_error_hndlr_default);
    }

    /* Check sanity of reporting level. */
    if (level > TRICK_ERROR_SILENT) {
        trick_error_report(error_hndlr, TRICK_ERROR_ALERT,
                           __FILE__, __LINE__,
                           "Unknown error level for " "trick_error_set_send_hs_flag; level unchanged.\n");
        return;
    }

    /* Set the appropriate use_send_hs flag */
    error_hndlr->use_send_hs[level] = use_send_hs;

    /* warn the user that send_hs has been replaced with fprintf */
    trick_error_report(error_hndlr, TRICK_ERROR_ALERT,
                       __FILE__, __LINE__,
                       "trick_error_set_send_hs_flag no longer supported.  All printouts will use fprintf\n");

    return;

}


void trick_error_set_all_send_hs_flags(TrickErrorHndlr * error_hndlr,   /* Inout: Error handler data */
                                       int use_send_hs)
{                                      /* In: 1= use send_hs;else use fprintf */
    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL) {
        error_hndlr = &(trick_error_hndlr_default);
    }

    /* Set the appropriate use_send_hs flag. */
    error_hndlr->use_send_hs[TRICK_ERROR_ALL] = use_send_hs;
    error_hndlr->use_send_hs[TRICK_ERROR_TRIVIAL] = use_send_hs;
    error_hndlr->use_send_hs[TRICK_ERROR_ADVISORY] = use_send_hs;
    error_hndlr->use_send_hs[TRICK_ERROR_CAUTION] = use_send_hs;
    error_hndlr->use_send_hs[TRICK_ERROR_WARNING] = use_send_hs;
    error_hndlr->use_send_hs[TRICK_ERROR_ALERT] = use_send_hs;
    error_hndlr->use_send_hs[TRICK_ERROR_FATAL] = use_send_hs;
    error_hndlr->use_send_hs[TRICK_ERROR_ABORT] = use_send_hs;
    error_hndlr->use_send_hs[TRICK_ERROR_SILENT] = use_send_hs;

    return;

}

void trick_error_copy_send_hs_flags(TrickErrorHndlr * dest_error_hndlr, /* OUT: Error handler data */
                                    TrickErrorHndlr * src_error_hndlr)
{                                      /* IN: -- Error handler data */
    memcpy((void *) dest_error_hndlr->use_send_hs, (void *) src_error_hndlr->use_send_hs, (sizeof(int) * 9));
}



TrickErrorFuncPtr trick_error_get_func( /* Return: Error function */
                                          TrickErrorHndlr * error_hndlr)
{                                      /* Inout: Error handler data. */

    if (error_hndlr == (TrickErrorHndlr *) NULL)
        return ((TrickErrorFuncPtr) NULL);
    else
        return (error_hndlr->error_func);
}


TrickErrorDataPtr trick_error_get_data( /* Return: Error data */
                                          TrickErrorHndlr * error_hndlr)
{                                      /* Inout: Error handler data. */

    if (error_hndlr == (TrickErrorHndlr *) NULL)
        return ((TrickErrorDataPtr) NULL);
    else
        return (error_hndlr->data_ptr);
}


TrickErrorLevel trick_error_get_report_level(   /* Return: Err report level */
                                                TrickErrorHndlr * error_hndlr)
{                                      /* Inout: Error handler data */

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL)
        return (TRICK_ERROR_ALL);
    else
        return (error_hndlr->report_level);
}


FILE *trick_error_get_stream(   /* Return: Err stream for report level */
                                TrickErrorHndlr * error_hndlr,  /* Inout: Error handler data */
                                TrickErrorLevel level)
{                                      /* In: Err report level stream to get */

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL)
        return ((FILE *) NULL);
    else {
        /* Check sanity of reporting level. */
        if (level > TRICK_ERROR_SILENT)
            return ((FILE *) NULL);
        else
            return (error_hndlr->report_stream[level]);
    }
}

/* Return: Err stream for report level in error_hndlr */
FILE *trick_error_get_curr_stream(TrickErrorHndlr * error_hndlr)
{                                      /* Inout: Error handler data */

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL)
        return ((FILE *) NULL);
    else {
        /* Check sanity of reporting level. */
        if (error_hndlr->report_level > TRICK_ERROR_SILENT)
            return ((FILE *) NULL);
        else
            return (error_hndlr->report_stream[error_hndlr->report_level]);
    }
}


int trick_error_get_send_hs_flag(       /* Return: send_hs flag for repor level */
                                    TrickErrorHndlr * error_hndlr,      /* Inout: Error handler data */
                                    TrickErrorLevel level)
{                                      /* In: Error repor level stream to get */

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL)
        return (0);
    else {
        /* Check sanity of reporting level. */
        if (level > TRICK_ERROR_SILENT)
            return (0);
        else
            return (error_hndlr->use_send_hs[level]);
    }
}

/* RETURN: -- send_hs flag for reporting level in error_hndlr */
int trick_error_get_curr_send_hs_flag(TrickErrorHndlr * error_hndlr)
{                                      /* Inout: Error handler data */
    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL)
        return (0);
    else {
        /* Check sanity of reporting level. */
        if (error_hndlr->report_level > TRICK_ERROR_SILENT)
            return (0);
        else
            return (error_hndlr->use_send_hs[error_hndlr->report_level]);
    }
}

#define TE_MAX_MSG_SIZE  4096

void trick_error_report(TrickErrorHndlr * error_hndlr,  /* In: Error object */
                        TrickErrorLevel error_level,    /* In: Error level for this error */
                        const char *file,     /* In: File in which error occured.  */
                        int line,       /* In: Line number where error occured. */
                        const char *format,   /* In: Error message format */
                        ...)
{                                      /* In: ** variable length arg list.  */
    va_list args;
    int msg_len;
    char message[TE_MAX_MSG_SIZE];


    va_start(args, format);

    /* Check for setting of default error handling object. */
    if (error_hndlr == (TrickErrorHndlr *) NULL) {
        error_hndlr = &(trick_error_hndlr_default);
    } else {
        if (error_hndlr->error_func == (TrickErrorFuncPtr) NULL) {
            error_hndlr->error_func = (TrickErrorFuncPtr) trick_error_func_default;
        }
    }

    msg_len = strlen(format);

    if (msg_len <= 0) {
        va_end(args);
        (*(error_hndlr->error_func)) (error_hndlr,
                                      TRICK_ERROR_ALERT, __FILE__, __LINE__, "Empty message for trick_error_report.\n");
        return;
    }

    vsprintf(message, format, args);
    va_end(args);

    /*
     * Call error handling function only if error level of this error
     * message is greater than the reporting level for this error handler.
     */
    if (error_level >= error_hndlr->report_level) {
        (*(error_hndlr->error_func)) (error_hndlr, error_level, file, line, message);
    }

    return;

}

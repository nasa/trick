/*
PURPOSE:      (Allows clients to get and set Trick parameters)
PROGRAMMERS: (((Keith Vetter) (LinCom) (September 2001) (--)))
*/

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits>
#include <udunits2.h>

#include "trick/parameter_types.h"
#include "trick/attributes.h"
#include "trick/bitfield_proto.h"
#include "trick/wcs_ext.h"
#include "trick/VariableServer.hh"
#include "trick/TrickConstant.hh"

/* PROTO */
size_t escape_str(const char *in_s, char *out_s);

#define MAX_VAL_STRLEN 2048

int vs_format_ascii(Trick::VariableReference * var, char *value) {

    /* for string types, return -1 if string is too big to fit in buffer (MAX_VAL_STRLEN) */
    REF2 * ref ;
    ref = var->ref ;
    std::string var_name = ref->reference;

    // handle returning an array
    int size = 0 ;
    value[0] = '\0' ;
    // data to send was copied to buffer in copy_sim_data
    void * buf_ptr = var->buffer_out ;
    while (size < var->size) {
        size += var->ref->attr->size ;

        switch (ref->attr->type) {

        case TRICK_CHARACTER:
            if (ref->attr->num_index == ref->num_index) {
                sprintf(value, "%s%d", value,(char)cv_convert_double(var->conversion_factor, *(char *)buf_ptr));
            } else {
                /* All but last dim specified, leaves a char array */
                escape_str((char *) buf_ptr, value);
                size = var->size ;
            }
            break;
        case TRICK_UNSIGNED_CHARACTER:
            if (ref->attr->num_index == ref->num_index) {
                sprintf(value, "%s%u", value,(unsigned char)cv_convert_double(var->conversion_factor,*(unsigned char *)buf_ptr));
            } else {
                /* All but last dim specified, leaves a char array */
                escape_str((char *) buf_ptr, value);
                size = var->size ;
            }
            break;

        case TRICK_WCHAR:{
                if (ref->attr->num_index == ref->num_index) {
                    sprintf(value, "%s%d", value,*(wchar_t *) buf_ptr);
                } else {
                    // convert wide char string char string
                    size_t len = wcs_to_ncs_len((wchar_t *)buf_ptr) + 1 ;
                    if (len > MAX_VAL_STRLEN) {
                        return (-1);
                    }
                    wcs_to_ncs((wchar_t *) buf_ptr, value, len);
                    size = var->size ;
                }
            }
            break;

        case TRICK_STRING:
            if ((char *) buf_ptr != NULL) {
                escape_str((char *) buf_ptr, value);
                size = var->size ;
            } else {
                value[0] = '\0';
            }
            break;

        case TRICK_WSTRING:
            if ((wchar_t *) buf_ptr != NULL) {
                // convert wide char string char string
                size_t len = wcs_to_ncs_len((wchar_t *)buf_ptr) + 1 ;
                if (len > MAX_VAL_STRLEN) {
                    return (-1);
                }
                wcs_to_ncs((wchar_t *) buf_ptr, value, len);
                size = var->size ;
            } else {
                value[0] = '\0';
            }
            break;

#if ( __linux | __sgi )
        case TRICK_BOOLEAN:
            sprintf(value, "%s%d", value,(unsigned char)cv_convert_double(var->conversion_factor,*(unsigned char *)buf_ptr));
            break;
#endif

        case TRICK_SHORT:
            sprintf(value, "%s%d", value, (short)cv_convert_double(var->conversion_factor,*(short *)buf_ptr));
            break;

        case TRICK_UNSIGNED_SHORT:
            sprintf(value, "%s%u", value,(unsigned short)cv_convert_double(var->conversion_factor,*(unsigned short *)buf_ptr));
            break;

        case TRICK_INTEGER:
        case TRICK_ENUMERATED:
#if ( __sun | __APPLE__ )
        case TRICK_BOOLEAN:
#endif
            sprintf(value, "%s%d", value, (int)cv_convert_double(var->conversion_factor,*(int *)buf_ptr));
            break;

        case TRICK_BITFIELD:
            sprintf(value, "%d", GET_BITFIELD(buf_ptr, ref->attr->size, ref->attr->index[0].start, ref->attr->index[0].size));
            break;

        case TRICK_UNSIGNED_BITFIELD:
            sprintf(value, "%u", GET_UNSIGNED_BITFIELD(buf_ptr, ref->attr->size, ref->attr->index[0].start, ref->attr->index[0].size));
            break;
        case TRICK_UNSIGNED_INTEGER:
            sprintf(value, "%s%u", value, (unsigned int)cv_convert_double(var->conversion_factor,*(unsigned int *)buf_ptr));
            break;

        case TRICK_LONG:
            sprintf(value, "%s%ld", value, (long)cv_convert_double(var->conversion_factor,*(long *)buf_ptr));
            break;

        case TRICK_UNSIGNED_LONG:
            sprintf(value, "%s%lu", value, (unsigned long)cv_convert_double(var->conversion_factor,*(unsigned long *)buf_ptr));
            break;

        case TRICK_FLOAT:
            sprintf(value, "%s%.8g", value, cv_convert_float(var->conversion_factor,*(float *)buf_ptr));
            break;

        case TRICK_DOUBLE:
            sprintf(value, "%s%.16g", value, cv_convert_double(var->conversion_factor,*(double *)buf_ptr));
            break;

        case TRICK_LONG_LONG:
            // This is a work-round for when terminate_time is not defined and is requested through variable server.
            // When sim terminate time is not defined, the related variable is the max of the type.
            // The unit conversion calculation will throw floating point exception.
            // For trick_sys.sched.terminate_time, there is no need to perform such conversion.
            if (!var_name.compare("trick_sys.sched.terminate_time")) {
                    sprintf(value, "%s%lld", value, *(long long *)buf_ptr);
            } else {
                    sprintf(value, "%s%lld", value, (long long)cv_convert_double(var->conversion_factor,*(long long *)buf_ptr));
            }
            break;

        case TRICK_UNSIGNED_LONG_LONG:
            sprintf(value, "%s%llu", value,(unsigned long long)cv_convert_double(var->conversion_factor,*(unsigned long long *)buf_ptr));
            break;

        case TRICK_NUMBER_OF_TYPES:
            sprintf(value, "BAD_REF" );
            break;

        default:{
                return (-1);
            }
        } // end switch

        if (size < var->size) {
        // if returning an array, continue array as comma separated values
            strcat(value, ",") ;
            buf_ptr = (void*) ((long)buf_ptr + var->ref->attr->size) ;
        }
    } //end while

    if (ref->units) {
        if ( ref->attr->mods & TRICK_MODS_UNITSDASHDASH ) {
            sprintf(value, "%s {--}", value);
        } else {
            sprintf(value, "%s {%s}", value, ref->units);
        }
    }

    return (0);
}


/**
 * Escape all of the non printable characters in a string.
 */
size_t escape_str(const char *in_s, char *out_s)
{
    size_t i, in_len, out_len;
    if (in_s == NULL) {
        out_s[0] = '\0';
        return 0;
    }
    in_len = strlen(in_s);
    out_len = 0;
    if (out_s != NULL) {
        out_s[0] = '\0';
    }
    for (i = 0; i < in_len; i++) {
        int ch = in_s[i];
        char work_s[6];

        if (isprint(ch)) {
            work_s[0] = ch;
            work_s[1] = '\0';
        } else {
            if (ch == '\a') {
                sprintf(work_s, "\\a");
            } else if (ch == '\b') {
                sprintf(work_s, "\\b");
            } else if (ch == '\f') {
                sprintf(work_s, "\\f");
            } else if (ch == '\n') {
                sprintf(work_s, "\\n");
            } else if (ch == '\r') {
                sprintf(work_s, "\\n");
            } else if (ch == '\t') {
                sprintf(work_s, "\\t");
            } else if (ch == '\v') {
                sprintf(work_s, "\\v");
            } else {
                sprintf(work_s, "\\x%02x", ch);
            }
        }
        out_len += strlen(work_s);
        if (out_s != NULL) {
            if (out_len < MAX_VAL_STRLEN) {
                strcat(out_s, work_s);
            } else {
                // indicate string is truncated because it's too big
                return -1;
            }
        }
    }

    return (out_len);
}

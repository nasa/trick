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
#include <sstream>

#include "trick/parameter_types.h"
#include "trick/attributes.h"
#include "trick/bitfield_proto.h"
#include "trick/wcs_ext.h"
#include "trick/VariableServer.hh"
#include "trick/TrickConstant.hh"

/* PROTO */
size_t escape_str(const char *in_s, char *out_s);

#define MAX_VAL_STRLEN 2048

int vs_format_ascii(Trick::VariableReference * var, char *value, size_t value_size) {

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
                snprintf(value, value_size, "%s%d", value,(char)cv_convert_double(var->conversion_factor, *(char *)buf_ptr));
            } else {
                /* All but last dim specified, leaves a char array */
                escape_str((char *) buf_ptr, value);
                size = var->size ;
            }
            break;
        case TRICK_UNSIGNED_CHARACTER:
            if (ref->attr->num_index == ref->num_index) {
                snprintf(value, value_size, "%s%u", value,(unsigned char)cv_convert_double(var->conversion_factor,*(unsigned char *)buf_ptr));
            } else {
                /* All but last dim specified, leaves a char array */
                escape_str((char *) buf_ptr, value);
                size = var->size ;
            }
            break;

        case TRICK_WCHAR:{
                if (ref->attr->num_index == ref->num_index) {
                    snprintf(value, value_size, "%s%d", value,*(wchar_t *) buf_ptr);
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
            snprintf(value, value_size, "%s%d", value,(unsigned char)cv_convert_double(var->conversion_factor,*(unsigned char *)buf_ptr));
            break;
#endif

        case TRICK_SHORT:
            snprintf(value, value_size, "%s%d", value, (short)cv_convert_double(var->conversion_factor,*(short *)buf_ptr));
            break;

        case TRICK_UNSIGNED_SHORT:
            snprintf(value, value_size, "%s%u", value,(unsigned short)cv_convert_double(var->conversion_factor,*(unsigned short *)buf_ptr));
            break;

        case TRICK_INTEGER:
        case TRICK_ENUMERATED:
#if ( __sun | __APPLE__ )
        case TRICK_BOOLEAN:
#endif
            snprintf(value, value_size, "%s%d", value, (int)cv_convert_double(var->conversion_factor,*(int *)buf_ptr));
            break;

        case TRICK_BITFIELD:
            snprintf(value, value_size, "%d", GET_BITFIELD(buf_ptr, ref->attr->size, ref->attr->index[0].start, ref->attr->index[0].size));
            break;

        case TRICK_UNSIGNED_BITFIELD:
            snprintf(value, value_size, "%u", GET_UNSIGNED_BITFIELD(buf_ptr, ref->attr->size, ref->attr->index[0].start, ref->attr->index[0].size));
            break;
        case TRICK_UNSIGNED_INTEGER:
            snprintf(value, value_size, "%s%u", value, (unsigned int)cv_convert_double(var->conversion_factor,*(unsigned int *)buf_ptr));
            break;

        case TRICK_LONG: {
            long l = *(long *)buf_ptr;
            if (var->conversion_factor != cv_get_trivial()) {
                l = (long)cv_convert_double(var->conversion_factor, l);
            }
            snprintf(value, value_size, "%s%ld", value, l);
            break;
        }

        case TRICK_UNSIGNED_LONG: {
            unsigned long ul = *(unsigned long *)buf_ptr;
            if (var->conversion_factor != cv_get_trivial()) {
                ul = (unsigned long)cv_convert_double(var->conversion_factor, ul);
            }
            snprintf(value, value_size, "%s%lu", value, ul);
            break;
        }

        case TRICK_FLOAT:
            snprintf(value, value_size, "%s%.8g", value, cv_convert_float(var->conversion_factor,*(float *)buf_ptr));
            break;

        case TRICK_DOUBLE:
            snprintf(value, value_size, "%s%.16g", value, cv_convert_double(var->conversion_factor,*(double *)buf_ptr));
            break;

        case TRICK_LONG_LONG: {
            long long ll = *(long long *)buf_ptr;
            if (var->conversion_factor != cv_get_trivial()) {
                ll = (long long)cv_convert_double(var->conversion_factor, ll);
            }
            snprintf(value, value_size, "%s%lld", value, ll);
            break;
        }

        case TRICK_UNSIGNED_LONG_LONG: {
            unsigned long long ull = *(unsigned long long *)buf_ptr;
            if (var->conversion_factor != cv_get_trivial()) {
                ull = (unsigned long long)cv_convert_double(var->conversion_factor, ull);
            }
            snprintf(value, value_size, "%s%llu", value, ull);
            break;
        }

        case TRICK_NUMBER_OF_TYPES:
            snprintf(value, value_size, "BAD_REF" );
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
        std::stringstream unit_str;
        if ( ref->attr->mods & TRICK_MODS_UNITSDASHDASH ) {
            unit_str << " {--}";
        } else {
            unit_str << " {" << ref->units << "}";

        }
        size_t max_copy_size = value_size - strlen(value) - 1;
        strncat(value, unit_str.str().c_str(), max_copy_size);
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
                snprintf(work_s, sizeof(work_s), "\\a");
            } else if (ch == '\b') {
                snprintf(work_s, sizeof(work_s), "\\b");
            } else if (ch == '\f') {
                snprintf(work_s, sizeof(work_s), "\\f");
            } else if (ch == '\n') {
                snprintf(work_s, sizeof(work_s), "\\n");
            } else if (ch == '\r') {
                snprintf(work_s, sizeof(work_s), "\\n");
            } else if (ch == '\t') {
                snprintf(work_s, sizeof(work_s), "\\t");
            } else if (ch == '\v') {
                snprintf(work_s, sizeof(work_s), "\\v");
            } else {
                snprintf(work_s, sizeof(work_s), "\\x%02x", ch);
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

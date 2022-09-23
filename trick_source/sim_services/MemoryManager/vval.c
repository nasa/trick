#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __Lynx__
#include <wchar.h>
#endif
#include "trick/vval.h"
#include "trick/value.h"
#include "trick/parameter_types.h"
#include "trick/wcs_ext.h"

/**
 */
char vval_char(V_DATA * V)
{
    switch (V->type) {
        case TRICK_CHARACTER:
        case TRICK_UNSIGNED_CHARACTER:
            return (V->value.c);
        case TRICK_SHORT:
        case TRICK_UNSIGNED_SHORT:
            return ((char) V->value.s);
        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
        case TRICK_UNSIGNED_INTEGER:
            return ((char) V->value.i);
        case TRICK_WCHAR:
#if __Lynx__
            return (0);
#else
            return ((char) V->value.wc);
#endif
        case TRICK_LONG:
        case TRICK_UNSIGNED_LONG:
            return ((char) V->value.l);
        case TRICK_LONG_LONG:
        case TRICK_UNSIGNED_LONG_LONG:
            return ((char) V->value.ll);
        case TRICK_FLOAT:
            return ((char) V->value.f);
        case TRICK_DOUBLE:
            return ((char) V->value.d);
        default :
            return(0);
    }
    return (0);
}

/**
 */
short vval_short(V_DATA * V)
{
    switch (V->type) {
        case TRICK_CHARACTER:
            return ((short) V->value.c);
        case TRICK_UNSIGNED_CHARACTER:
            return ((short) (unsigned char) V->value.c);
        case TRICK_SHORT:
        case TRICK_UNSIGNED_SHORT:
            return (V->value.s);
        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
        case TRICK_UNSIGNED_INTEGER:
            return ((short) V->value.i);
        case TRICK_WCHAR:
#if __Lynx__
            return (0);
#else
            return ((short) V->value.wc);
#endif
        case TRICK_LONG:
        case TRICK_UNSIGNED_LONG:
            return ((short) V->value.l);
        case TRICK_LONG_LONG:
        case TRICK_UNSIGNED_LONG_LONG:
            return ((short) V->value.ll);
        case TRICK_FLOAT:
            return ((short) V->value.f);
        case TRICK_DOUBLE:
            return ((short) V->value.d);
        default :
            return(0);
    }
    return (0);
}

/**
 */
int vval_int(V_DATA * V)
{
    switch (V->type) {
        case TRICK_CHARACTER:
            return ((int) V->value.c);
        case TRICK_UNSIGNED_CHARACTER:
            return ((int) (unsigned char) V->value.c);
        case TRICK_SHORT:
            return ((int) (short) V->value.s);
        case TRICK_UNSIGNED_SHORT:
            return ((int) (unsigned short) V->value.s);
        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
        case TRICK_UNSIGNED_INTEGER:
            return (V->value.i);
        case TRICK_WCHAR:
#if __Lynx__
            return (0);
#else
            return ((int) V->value.wc);
#endif
        case TRICK_LONG:
        case TRICK_UNSIGNED_LONG:
            return ((int) V->value.l);
        case TRICK_LONG_LONG:
        case TRICK_UNSIGNED_LONG_LONG:
            return ((int) V->value.ll);
        case TRICK_FLOAT:
            return ((int) V->value.f);
        case TRICK_DOUBLE:
            return ((int) V->value.d);
        default :
            return(0);
    }
    return (0);
}

#ifndef __Lynx__
/**
 */
wchar_t vval_wchar(V_DATA * V)
{
    switch (V->type) {
        case TRICK_CHARACTER:
            return ((wchar_t) V->value.c);
        case TRICK_UNSIGNED_CHARACTER:
            return ((wchar_t) (unsigned char) V->value.c);
        case TRICK_SHORT:
            return ((wchar_t) (short) V->value.s);
        case TRICK_UNSIGNED_SHORT:
            return ((wchar_t) (unsigned short) V->value.s);
        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
        case TRICK_UNSIGNED_INTEGER:
            return ((wchar_t) V->value.i);
        case TRICK_WCHAR:
            return (V->value.wc);
        case TRICK_LONG:
        case TRICK_UNSIGNED_LONG:
            return ((wchar_t) V->value.l);
        case TRICK_LONG_LONG:
        case TRICK_UNSIGNED_LONG_LONG:
            return ((wchar_t) V->value.ll);
        case TRICK_FLOAT:
            return ((wchar_t) V->value.f);
        case TRICK_DOUBLE:
            return ((wchar_t) V->value.d);
        default :
            return(0);
    }
    return (0);
}
#else
int vval_wchar(V_DATA * V)
{
    return (0);
}
#endif

/**
 */
long vval_long(V_DATA * V)
{
    switch (V->type) {
        case TRICK_CHARACTER:
            return ((long) V->value.c);
        case TRICK_UNSIGNED_CHARACTER:
            return ((long) (unsigned char) V->value.c);
        case TRICK_SHORT:
            return ((long) (short) V->value.s);
        case TRICK_UNSIGNED_SHORT:
            return ((long) (unsigned short) V->value.s);
        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
            return ((long) V->value.i);
        case TRICK_UNSIGNED_INTEGER:
            return ((long) (unsigned int) V->value.i);
        case TRICK_WCHAR:
#if __Lynx__
            return (0);
#else
            return ((long) V->value.wc);
#endif
        case TRICK_LONG:
        case TRICK_UNSIGNED_LONG:
            return ((long) V->value.l);
        case TRICK_LONG_LONG:
        case TRICK_UNSIGNED_LONG_LONG:
            return ((long) V->value.ll);
        case TRICK_FLOAT:
            return ((long) V->value.f);
        case TRICK_DOUBLE:
            return ((long) V->value.d);
        case TRICK_VOID_PTR:
            return ((long) V->value.vp);
        default :
            return(0);
    }
    return (0);
}

/**
 */
long long vval_longlong(V_DATA * V)
{
    switch (V->type) {
        case TRICK_CHARACTER:
            return ((long long) V->value.c);
        case TRICK_UNSIGNED_CHARACTER:
            return ((long long) (unsigned char) V->value.c);
        case TRICK_SHORT:
            return ((long long) V->value.s);
        case TRICK_UNSIGNED_SHORT:
            return ((long long) (unsigned short) V->value.s);
        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
            return ((long long) V->value.i);
        case TRICK_UNSIGNED_INTEGER:
            return ((long long) (unsigned int) V->value.i);
        case TRICK_WCHAR:
#if __Lynx__
            return (0);
#else
            return ((long long) V->value.wc);
#endif
        case TRICK_LONG:
            return ((long long) V->value.l);
        case TRICK_UNSIGNED_LONG:
            return ((long long) (unsigned long) V->value.l);
        case TRICK_LONG_LONG:
        case TRICK_UNSIGNED_LONG_LONG:
            return (V->value.ll);
        case TRICK_FLOAT:
            return ((long long) V->value.f);
        case TRICK_DOUBLE:
            return ((long long) V->value.d);
        default :
            return(0);
    }
    return (0);
}

/**
 */
float vval_float(V_DATA * V)
{
    switch (V->type) {
        case TRICK_CHARACTER:
            return ((float) V->value.c);
        case TRICK_UNSIGNED_CHARACTER:
            return ((float) (unsigned char) V->value.c);
        case TRICK_SHORT:
            return ((float) V->value.s);
        case TRICK_UNSIGNED_SHORT:
            return ((float) (unsigned short) V->value.s);
        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
            return ((float) V->value.i);
        case TRICK_UNSIGNED_INTEGER:
            return ((float) (unsigned int) V->value.i);
        case TRICK_LONG:
            return ((float) V->value.l);
        case TRICK_UNSIGNED_LONG:
            return ((float) (unsigned long) V->value.l);
        case TRICK_LONG_LONG:
            return ((float) V->value.ll);
        case TRICK_UNSIGNED_LONG_LONG:
            return ((float) (unsigned long long) V->value.ll);
        case TRICK_FLOAT:
            return (V->value.f);
        case TRICK_DOUBLE:
            return (V->value.d);
        default :
            return(0.0);
    }
    return (0.0);
}

/**
 */
double vval_double(V_DATA * V)
{
    switch (V->type) {
        case TRICK_CHARACTER:
            return ((double) V->value.c);
        case TRICK_UNSIGNED_CHARACTER:
            return ((double) (unsigned char) V->value.c);
        case TRICK_STRING:
            return ((double) (long) V->value.cp);
        case TRICK_SHORT:
            return ((double) V->value.s);
        case TRICK_UNSIGNED_SHORT:
            return ((double) (unsigned short) V->value.s);
        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
            return ((double) V->value.i);
        case TRICK_UNSIGNED_INTEGER:
            return ((double) (unsigned int) V->value.i);
        case TRICK_LONG:
            return ((double) V->value.l);
        case TRICK_UNSIGNED_LONG:
            return ((double) (unsigned long) V->value.l);
        case TRICK_LONG_LONG:
            return ((double) V->value.ll);
        case TRICK_UNSIGNED_LONG_LONG:
            return ((double) (unsigned long long) V->value.ll);
        case TRICK_FLOAT:
            return (V->value.f);
        case TRICK_DOUBLE:
            return (V->value.d);
        case TRICK_FILE_PTR:
            return ((double) (long) V->value.fp);
        case TRICK_VOID_PTR:
            return ((double) (long) V->value.vp);
        default :
            return(0.0);
    }
    return (0.0);
}

/**
 */
char *vval_string(V_DATA * V)
{
    switch (V->type) {
        case TRICK_STRING:
            return (V->value.cp);
        case TRICK_VOID_PTR:
            return ((char *) V->value.vp);
        default :
            return(0);
    }
    return (NULL);
}

/**
 */
wchar_t *vval_wstring(V_DATA * V)
{
    switch (V->type) {
        case TRICK_STRING:{

#if 0
// FIXME
                wchar_t *wcs;
                size_t wcs_size;

                if (V->value.cp) {
                    /* The narrow string to be converted is not NULL so, figure how much space we are going to need to
                       store the resulting wide string. */
                    wcs_size = ncs_to_wcs_len(V->value.cp) + 1;
                    /* and allocate that much. */
                    if ((wcs = (wchar_t *) ip_alloc_type((int) wcs_size, (int) sizeof(wchar_t), NULL, NULL)) == NULL) {
                        fprintf(stderr, "Out of memory at %s:%d.\n", __FILE__, __LINE__);
                        fflush(stderr);
                        exit(1);
                    }
                    /* Convert the narrow string to a wide-string, storing it in the space that we just allocated. */
                    ncs_to_wcs(V->value.cp, wcs, wcs_size);
                    return wcs;
                } else {
                    /* the string is NULL */
                    return (wchar_t *) NULL;
                }
#endif
            }
        case TRICK_WSTRING:
#if __Lynx__
            return (NULL);
#else
            return (V->value.wcp);
#endif
        case TRICK_VOID_PTR:
            return ((wchar_t *) V->value.vp);
        default :
            return(NULL);
    }
    return (NULL);
}

/**
 */
FILE *vval_filep(V_DATA * V)
{
    switch (V->type) {
        case TRICK_FILE_PTR:
            return (V->value.fp);
        default :
            return(NULL);
    }
    return (NULL);
}

/**
 */
void *vval_voidp(V_DATA * V)
{
    switch (V->type) {
        case TRICK_CHARACTER:
            return ((void *) (long) V->value.c);
        case TRICK_UNSIGNED_CHARACTER:
            return ((void *) (long) V->value.c);
        case TRICK_SHORT:
            return ((void *) (long) V->value.s);
        case TRICK_UNSIGNED_SHORT:
            return ((void *) (long) V->value.s);
        case TRICK_ENUMERATED:
        case TRICK_INTEGER:
            return ((void *) (long) V->value.i);
        case TRICK_UNSIGNED_INTEGER:
            return ((void *) (long) V->value.i);
        case TRICK_LONG:
            return ((void *) V->value.l);
        case TRICK_UNSIGNED_LONG:
            return ((void *) V->value.l);
        case TRICK_LONG_LONG:
            return ((void *) (long) V->value.ll);
        case TRICK_UNSIGNED_LONG_LONG:
            return ((void *) (long) V->value.ll);
        case TRICK_STRING:
            return ((void *) V->value.cp);
        case TRICK_WSTRING:
#if __Lynx__
            return (NULL);
#else
            return ((void *) V->value.wcp);
#endif
        case TRICK_FILE_PTR:
            return ((void *) V->value.fp);
        case TRICK_VOID_PTR:
            return ((void *) V->value.vp);
        default :
            return(NULL);
    }
    return (NULL);
}

/**
 */
void delete_v_tree(V_TREE* v_tree) {
    V_TREE* next;
    V_TREE* current = v_tree;
    while(current) {
        if (current->down) {
            delete_v_tree( current->down);
        } else if (current->v_data) {
            free(current->v_data);
        }
        next = current->next;
        free( current);
        current = next;
    }
}

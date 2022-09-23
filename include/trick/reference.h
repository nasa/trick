/*
PURPOSE:
    (Data structure typedef for parameter attributes used by executive IO
        routines and filled by the interface code generator.)

PROGRAMMERS:
    ((Robert W. Bailey) (LinCom Corp) (September 1990) (Initial Release.))
*/

#ifndef REFERENCE_H
#define REFERENCE_H

#include "trick/attributes.h"
#include "trick/value.h"
#include "trick/dllist.h"

// FIXME These ERROR CODES
#define TRICK_NO_ERROR 0
#define TRICK_PARAMETER_ARRAY_SIZE 6
#define TRICK_PARAMETER_ADDRESS_NULL 7
#define TRICK_UNITS_CONVERSION_ERROR 8

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    REF_ADDRESS = 0,
    REF_VALUE   = 1,
    REF_INVALID = 99
} REF_TYPE;

typedef enum {
    AO_ADDRESS ,
    AO_OFFSET ,
    AO_DEREFERENCE
} ADDRESS_OPERATOR ;

typedef union {
    void * address ;
    long offset ;
} ADDRESS_OPERAND ;

typedef struct {
    ADDRESS_OPERATOR operator_ ;
    ADDRESS_OPERAND operand ;
} ADDRESS_NODE ;

/**
 * The REF2 data structure represents a value or a reference to a value.
 */
typedef struct {
    char* reference;        /**< -- Main buffer for variable reference */
    int   num_index;        /**< -- num index for last level of reference */
    char* units;            /**< -- Units as specified in input runstream */
    int   num_index_left;   /**< -- FIXME OBSOLETE Number of remaining indicies to specify */
    int   pointer_present;  /**< -- 0 = no , 1 = yes (address could change) */
    int   ref_type;         /**< -- 0 = address, 1 = value. */
    void* address;          /**< ** Address of the specified reference */
    V_DATA v_data;          /**< ** Value */
    ATTRIBUTES* attr;       /**< -- Parameter attributes */
    ATTRIBUTES* ref_attr;   /**< -- Dynamically allocated reference attribute. */
    int create_add_path ;   /**< ** bool to shortcut to resolve address */
    DLLIST * address_path ; /**< ** shortcut to resolve address */
} REF2;

/**
 * Get the value of the variable referenced by R.
 * @param R variable reference.
 * @param V pointer to V_DATA object into which the value is to be assigned.
 * @todo isn't this obsolete?
 */
int ref_to_value( REF2* R, V_DATA* V);

#ifdef __cplusplus
}
#endif
#endif

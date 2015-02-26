/*******************************************************************************
*                                                                              *
* Trick Simulation Environment Software                                        *
*                                                                              *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                      *
* All rights reserved.                                                         *
*                                                                              *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized    *
* use of Trick Software including source code, object code or executables is   *
* strictly prohibited and LinCom assumes no liability for such actions or      *
* results thereof.                                                             *
*                                                                              *
* Trick Software has been developed under NASA Government Contracts and        *
* access to it may be granted for Government work by the following contact:    *
*                                                                              *
* Contact: Charles Gott, Branch Chief                                          *
*          Simulation and Graphics Branch                                      *
*          Automation, Robotics, & Simulation Division                         *
*          NASA, Johnson Space Center, Houston, TX                             *
*                                                                              *
*******************************************************************************/
/*
PURPOSE:
    (Data structure typedef for parameter attributes used by executive IO
        routines and filled by the interface code generator.)

PROGRAMMERS:
    ((Robert W. Bailey) (LinCom Corp) (September 1990) (Initial Release.))
*/

/*
 * $Id: reference.h 3627 2014-08-12 23:21:09Z jpenn1 $
 */

#ifndef _REFERENCE_H_
#define _REFERENCE_H_

#include "sim_services/MemoryManager/include/attributes.h"
#include "sim_services/MemoryManager/include/value.h"
#include "trick_utils/trick_adt/include/dllist.h"

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

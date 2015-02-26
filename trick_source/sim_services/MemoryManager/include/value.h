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
    (Define data structure which holds list of data values during executive
        parsing of the input file.)

PROGRAMMERS:
    ((Robert W. Bailey) (LinCom Corp) (September 1990) (Initial Release.))
*/

/*
 * $Id: value.h 646 2010-04-28 21:50:31Z lin $
 */

#ifndef VALUE_H_DEFINED
#define VALUE_H_DEFINED


#include <stdio.h>

#ifndef __Lynx__
#include <wchar.h>
#endif

#include "sim_services/MemoryManager/include/parameter_types.h"

#define MAX_VALUE 512

#ifdef __cplusplus
extern "C" {
#endif

typedef union {

    char c;         /* ** characters */
    char *cp;       /* ** char* (strings) */
#if !( __Lynx__ | SWIG )
    wchar_t wc;     /* ** wide characters */
    wchar_t *wcp;   /* ** wchar_t * (wide strings) */
#endif
    short s;        /* ** short */
    FILE *fp;       /* ** FILE pointers */
    int i;          /* ** int */
    long l;         /* ** long */
    float f;        /* ** floats */
    double d;       /* ** double */
    long long ll;   /* -- long long */
    void *vp;       /* ** void pointer */

} VALUE;

/**
 * Represents a typed value.
 */
typedef struct {
    TRICK_TYPE type;     /**< -- value type */
    VALUE      value;    /**< -- value */
} V_DATA;

typedef struct {
    int num_val;    /* -- Number of values */
    V_DATA v_data[MAX_VALUE];
} VALUE_LIST;

/**
 Represents a collection of data. It may represent a single value or
 it may represent data array of up to 8 dimensions.
 @dot
 digraph V_TREE {
    { node 
        [ shape = plaintext,
          label = "V_TREE*\n(to initialize a \[3\]\[2\] array )"]
        p
    }
    { node 
        [shape = record, label="{V_TREE|{<f0>data|<f1>down|<f2>next}}" ]
        n0 n1 n2
        n00 n01 n10 n11 n20 n21
    }
    { rank=same; n0 n1 n2}
    { rank=same; n00 n01 n10 n11 n20 n21 }
    p -> n0
    "n0":f2 -> n1
    "n1":f2 -> n2
    "n0":f1 -> n00
    "n00":f2 -> n01
    "n1":f1 -> n10
    "n10":f2 -> n11
    "n2":f1 -> n20
    "n20":f2 -> n21
    { node [shape=record]
        d00 [label="V_DATA"]
        d01 [label="V_DATA"]
        d10 [label="V_DATA"]
        d11 [label="V_DATA"]
        d20 [label="V_DATA"]
        d21 [label="V_DATA"]
    }
    "n00":f0 -> d00 [label="\  \[0\]\[0\]"]
    "n01":f0 -> d01 [label="\  \[0\]\[1\]"]
    "n10":f0 -> d10 [label="\  \[1\]\[0\]"]
    "n11":f0 -> d11 [label="\  \[1\]\[1\]"]
    "n20":f0 -> d20 [label="\  \[2\]\[0\]"]
    "n21":f0 -> d21 [label="\  \[2\]\[1\]"]
 }
 @enddot
 */
typedef struct v_tree {
    struct v_tree* last; /**< -- last element in the current list */
    struct v_tree* next; /**< -- next element in the current list */
    struct v_tree* down; /**< -- subordinate list of the current element. If non-NULL then "v_data" should be NULL.*/
    V_DATA* v_data;        /**< -- data of the current element. If non-NULL then "down" should be NULL.*/
} V_TREE;

#ifdef __cplusplus
}
#endif
#endif

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
 * $Id: var.h 958 2010-08-13 18:38:19Z penn $
 *
 */

#include "sim_services/MemoryManager/include/value.h"

#ifndef _VAR_H_
#define _VAR_H_

/**
 * VAR_DECLARE represents a declarator.
 */
typedef struct {
    int num_pointers;         /**< ** Number of pointers (asterisks). */
    char *name;               /**< ** Name of variable. */
    VALUE_LIST *dim_list;     /**< ** Fixed (Constrained) dimensions. */
    V_DATA value;             /**< ** Initial value for variable */
} VAR_DECLARE;

/**
 VAR_LIST represents a declarator list.
 */
typedef struct {
    int num_vars;             /**< ** number of declarators in list */
    VAR_DECLARE *var_declare; /**< ** list of declarators. */
} VAR_LIST;

#endif

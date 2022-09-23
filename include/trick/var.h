#ifndef VAR_H
#define VAR_H
/*
    PURPOSE: ( Variable declarator structures. )
*/

#include "trick/value.h"

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

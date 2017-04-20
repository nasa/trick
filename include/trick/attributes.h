#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H

/*
    PURPOSE: ( Defines data-structures that Trick uses to describe
               C/C++ data-types found in a simulation header files.)
*/

#include <stddef.h>
#include "trick/parameter_types.h"

#define TRICK_VAR_OUTPUT 0x01
#define TRICK_VAR_INPUT 0x02
#define TRICK_CHKPNT_OUTPUT 0x04
#define TRICK_CHKPNT_INPUT 0x08
#define TRICK_MODS_UNITSDASHDASH 0x04

#define TRICK_MAX_INDEX 8

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    Language_C,     /* -- ANSI C */
    Language_CPP    /* -- C++ */
} Language;

/**
 * Enumeration Attributes (also see description of ATTRIBUTES.attr. )
 */
typedef struct {

    const char *label;    /**< --  Enumeration label */
    int value;      /**< --  Enumeration value */
    int mods;       /**< --  bit 32 = deprecated var */

} ENUM_ATTR;

/**
 * The INDEX structure is used (only) in the ATTRIBUTES structure to describe
 * array dimension information and bit-field information.
 */
typedef struct {
    int size;       /**< -- If ATTRIBUTES.type == TRICK_BITFIELD or ATTRIBUTES.type ==
                            TRICK_UNSIGNED_BITFIELD, then ATTRIBUTES.index.size[0]
                            represents the number of bits in the bitfield.

                            Otherwise:

                            0 indicates that the array is dynamic, i.e.,
                            may have any natural number of array elements.

                            > 0 indicates that the array is fixed (constrained).
                            The value specifies the maximum number of elements in
                            the array.  */
    int start;      /**< -- If ATTRIBUTES.type == TRICK_BITFIELD or ATTRIBUTES.type ==
                            TRICK_UNSIGNED_BITFIELD, then start represents the bit position
                            (in the range 0:31) of the most significant bit of the bit field.*/
} INDEX;

/**
 * ATTRIBUTES are data structures that represent detailed descriptions of data
 * types, including how an instance of a datatype is represented in memory.
 * ATTRIBUTES are fundemental to the design of Trick's Memory Management.
 *
 * The ICG code generator generates arrays of ATTRIBUTES, of varying lengths,
 * for each of the structured datat types that it finds in a header (.h or .hh)
 * file. These generated ATTRIBUTES are stored in a directory entitled ./io_src.
 * A generated file is named by prepending io_ to the name of the file from which
 * it is derived, and by changing the extension from .h or .hh to .c or .cpp
 * respectively.
 */
typedef struct ATTRIBUTES_tag {

    const char * name;         /**< -- Name of the variable of variable element of a structure. */
    const char * type_name;    /**< -- Character string representation of the C/C++ data type or user-defined type. */
    const char * units;        /**< -- Units of measurement. Units string specification as described WHERE? */
    const char * alias;        /**< -- OBSOLETE. */
    const char * user_defined; /**< -- OBSOLETE. */
    const char * des;          /**< -- OBSOLETE. Parameter description */
    int io;                    /**< -- Input / Output processing restrictions,

                                Each bit controls different I/O access.

                                Bit 31 TRICK_VAR_OUTPUT - Allows output through attribute based capabilities
                                Bit 30 TRICK_VAR_INPUT - Allows input through attribute based capabilities
                                Bit 29 TRICK_CHKPNT_OUTPUT - Allows checkpoint output
                                Bit 28 TRICK_CHKPNT_OUTPUT - Allows checkpoint input

                                */
    TRICK_TYPE type;    /**< -- TRICK data type. */
    int size;           /**< -- Data type size in bytes */
    double range_min;   /**< -- Minimum allowable value for parameter */
    double range_max;   /**< -- Minimum allowable value for parameter */
    Language language;  /**< -- Native language of parameter. */
    int mods;           /**< -- Modification bits.
                                bit 0 = c++ reference var
                                bit 1 = c++ static var
                                bit 2 = "--" units
                                bit 31 = deprecated var */

    long offset;        /**< -- Offset in bytes of this parameter from beginning of data structure */
    void *attr;         /**< -- Address to next level parameter attributes

                                          If type == TRICK_STRUCTURED, then attr points to the array
                                          of type ATTRIBUTES whose name is the catenation of "attr"
                                          and the name of the struct or class name.

                                          If type == TRICK_ENUMERATED, then attr points to the array
                                          of type ENUM_ATTR whose name is the catenation of "enum"
                                          and the name of the enum type.
                              */
    int num_index;      /**< -- Number of array indices for the parameter. May have values in the range 0..8.

                                0 = not arrayed.

                                1..8 = number of array dimensions.

                                NOTE: TRICK_BITFIELD and TRICK_UNSIGNED_BITFIELD are not considered
                                to be arrays so num_index is 0 for these types. Even so, for these
                                two types index[0] contains bitfield information. */
    INDEX index[TRICK_MAX_INDEX]; /**< -- An array of array-index information or bit-field information.
                                    Is only meaningful if num_index > 0 or if type is a bit field type. */

    void (*checkpoint_stl)(void * start_address, const char * obj_name , const char * var_name) ;
    void (*post_checkpoint_stl)(void * start_address, const char * obj_name , const char * var_name) ;
    void (*restore_stl)(void * start_address, const char * obj_name , const char * var_name) ;
    void (*clear_stl)(void * start_address) ;

} ATTRIBUTES;

typedef struct {

    char *type_name;
    ENUM_ATTR *E;

} ENUM_LIST;

typedef struct {

    char *name;
    ATTRIBUTES *attr;
    int size;

} NAME_TO_ATTR;

typedef struct {

    ATTRIBUTES *attr; /* ** Attributes of the typedeffed struct */
    ATTRIBUTES *base; /* ** Attributes of base struct */

} EQUIV_ATTRIBUTES ;

//void *trick_bswap_buffer(void *out, void *in, ATTRIBUTES * attr, int tofrom) ;

#ifdef __cplusplus
}
#endif
#endif

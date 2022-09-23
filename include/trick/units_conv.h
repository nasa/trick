/*
PURPOSE: (Units Conversion Package C Language Interface)

PROGRAMMERS:
    (((John M. Penn) (L-3Com/Titan) (May 2006) (v1.0)
      (Initial Release)))
*/

#ifndef UNITS_CONV_H
#define UNITS_CONV_H

/* --- Grammar for units specification strings ---
 *
 * units_specifier ->   units_product / units_specifier
 *                    | units_product
 *                    .
 *
 * units_product ->   units_factor * product
 *                  | units_factor product
 *                  | units_factor
 *                  .
 *
 * units_factor ->   units_primitive 3
 *                 | units_primitive 2
 *                 | units_primitive   // there is an implied exponent of 1 here.
 *                 .
 *
 * units_primitive ->  "1"   | "--"  | "am"  | "amp" | "as"  | "atm" | "BTU" | "C"   | "cnt" | "d"
 *	   	     | "dB"  | "F"   | "floz"| "ft"  | "g"   | "gal" | "hp"  | "Hz"  | "in"  | "inHg" | "inWater" | "J"
 *		     | "K"   | "kft" | "l"   | "lbf" | "lbm" | "m"   | "mi"  | "mmHg"| "mmWater" | "mol" | "mt"
 *		     | "N"   | "n.m."| "ohm" | "one" | "oz"  | "Pa"  | "psi" | "r"   | "R"   | "rev"
 *		     | "sl"  | "TNT" | "v"   | "W"   | "yd"
 */

#include <stdlib.h>
#ifndef CONV_ALLOC
#define CONV_ALLOC(number, size) calloc(number, size)
#endif

#ifndef CONV_FREE
#define CONV_FREE(p) free(p)
#endif

#define MAX_TRIADS   24
#define MAX_UNITS_SPEC_STRLEN 32

#ifdef __cplusplus
extern "C" {
#endif

typedef enum { CONV_OK, CONV_ERR } Conv_Status_t;

typedef struct {
    int op;                    /* -- operator */
    int a1;                    /* -- arg1 */
    int a2;                    /* -- arg2 */
} triad_t;

typedef struct {
    int   nt;                  /* -- Number of Triads */
    triad_t t[MAX_TRIADS];     /* -- triad array */
} Units_t;

typedef struct {
    double C[2];               /* -- Conversion Function Coefficients */
} UnitsConvFn_t;

/** @defgroup UNITS_C_INTF  "Units C Interface"
    $TRICK_HOME/trick_source/trick_utils/units/include/units_conv.h
 */

/**
 * @ingroup UNITS_C_INTF
 * Allocate and initialize a new Units_t according to the given units specifier.
 */
Units_t *new_units (const char *units_s);

/**
 * @ingroup UNITS_C_INTF
 * Create a conversion object (UnitsConvFn_t) that converts from f_units to t_units.
 */
Conv_Status_t conv_fn_u(const Units_t *f_units,     /* IN  */
                        const Units_t *t_units,     /* IN  */
                        UnitsConvFn_t *conv_fn );   /* OUT */

/**
 * @ingroup UNITS_C_INTF
 * Create a conversion object (UnitsConvFn_t) that converts from f_units to t_units.
 */
Conv_Status_t conv_fn_s(const char    *f_units_s,   /* IN  */
                        const char    *t_units_s,   /* IN  */
                        UnitsConvFn_t *conv_fn );   /* OUT */

/**
 * @ingroup UNITS_C_INTF
 * Perform a units conversion by applying conv_fn to from_value.
 */
double convert_units(double from_value, const UnitsConvFn_t *conv_fn );

/**
 * @ingroup UNITS_C_INTF
 * Convert an old (pre Trick 07) units specifier to a new units specifier.
 */
int convert_units_spec (const char *old_units_spec, char *new_units_spec);

/**
 * @ingroup UNITS_C_INTF
 * Print all known units primitives to stdout.
 */
void list_units (void) ;

#ifdef __cplusplus
}
#endif

#endif


/*
PURPOSE: (Define the Error conditions and error messages for the
          trick math routines)
PROGRAMMERS: (((Alex Lin) (NASA) (8/02) () (--)))
*/

#ifndef TRICK_MATH_ERROR_H
#define TRICK_MATH_ERROR_H

#define TM_SUCCESS     0
#define TM_SING_123_P  1
#define TM_SING_123_N  2
#define TM_SING_132_P  3
#define TM_SING_132_N  4
#define TM_SING_213_P  5
#define TM_SING_213_N  6
#define TM_SING_231_P  7
#define TM_SING_231_N  8
#define TM_SING_312_P  9
#define TM_SING_312_N  10
#define TM_SING_321_P  11
#define TM_SING_321_N  12
#define TM_ANG_NAN     13
#define TM_INV_ROT_SEQ 14
#define TM_DIAG_SMALL  15
#define TM_ZERO_DET    16

void tm_print_error( int error) ;
#endif

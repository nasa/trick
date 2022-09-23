/*
   PURPOSE: (Print out math error message)
   PROGRAMMERS: ((Alex Lin) (NASA) (8/02))
 */

#include <stdio.h>
#include "trick/trick_math_error.h"

static char *trick_math_errors[] = {
    /* 00 */ "Success",
    /* 01 */ "WARNING: Singularity in deuler_123() at PITCH = 90.0 deg, YAW angle set to 0.0",
    /* 02 */ "WARNING: Singularity in deuler_123() at PITCH = -90.0 deg, YAW angle set to 0.0",
    /* 03 */ "WARNING: Singularity in deuler_132() at YAW = 90.0 deg, PITCH angle set to 0.0",
    /* 04 */ "WARNING: Singularity in deuler_132() at YAW = -90.0 deg, PITCH angle set to 0.0",
    /* 05 */ "WARNING: Singularity in deuler_213() at ROLL = 90.0 deg, YAW angle set to 0.0",
    /* 06 */ "WARNING: Singularity in deuler_213() at ROLL = -90.0 deg, YAW angle set to 0.0",
    /* 07 */ "WARNING: Singularity in deuler_231() at YAW = 90.0 deg, ROLL angle set to 0.0",
    /* 08 */ "WARNING: Singularity in deuler_231() at YAW = -90.0 deg, ROLL angle set to 0.0",
    /* 09 */ "WARNING: Singularity in deuler_312() at ROLL = 90.0 deg, PITCH angle set to 0.0",
    /* 10 */ "WARNING: Singularity in deuler_312() at ROLL = -90.0 deg, PITCH angle set to 0.0",
    /* 11 */ "WARNING: Singularity in deuler_321() at PITCH = 90.0 deg, ROLL angle set to 0.0",
    /* 12 */ "WARNING: Singularity in deuler_321() at PITCH = -90.0 deg, ROLL angle set to 0.0",
    /* 13 */ "WARNING: Passed value is beyond the range of the asin function, Euler angles set to NANs",
    /* 14 */ "ERROR: Invalid Euler Rotation Sequence Option",
    /* 15 */ "ERROR: Diagonal Element is too small",
    /* 16 */ "ERROR: 3x3 matrix has zero determinant"
};

/* ENTRY POINT: */
void tm_print_error(int error)
{                                      /* In: Error number */
    fprintf(stderr, "%s\n", trick_math_errors[error]);

    return;
}

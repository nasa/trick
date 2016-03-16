#ifndef JD_H
#define JD_H
/**
 @file JD.h
 @brief  the amalgomated, gonkulated JD.h file

 Here is a detailed description.  Here is a detailed description.
 Here is a detailed description.  Here is a detailed description.
 Here is a detailed description.  Here is a detailed description.
 Here is a detailed description.  Here is a detailed description.
 Here is a detailed description.
 */
#include <stdio.h>
#include <math.h>

#ifdef __cplusplus
extern "C" {
#endif
/**
 * Returns 1 if the specified date is a Gregorian date.
 * @param Y (IN)  Year
 * @param M (IN)  Month
 * @param D (IN)  Day
 */
int is_gregorian_date (int Y, int M, double D);
/**
 * Returns 1 if the specified date is a Julian date.
 * @param Y (IN)  Year
 * @param M (IN)  Month
 * @param D (IN)  Day
 */
int is_julian_date (int Y, int M, double D);
/**
 * Calculates the Julian date from the given calendar date.
 * @param Y  (IN)  Year
 * @param M  (IN)  Month
 * @param D  (IN)  Day
 * @param JD (OUT) Julian date
 */
int Calendar_Date_to_JD (int Y, int M, double D, double * JD);
void JD_to_Calendar_Date (double JD, int *year, int *month, double *day) ;
#ifdef __cplusplus
}
#endif

#endif




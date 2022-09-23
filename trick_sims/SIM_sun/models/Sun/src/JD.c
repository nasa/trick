/*******************************************************************
 * @\file JD.c
 * @\brief This is the JD.c file
 * Reference(s):
 * (1) Meeus,Jean."Astronomical Algorithms",
 * 2nd Edition, Willmann-Bell, Inc.
 * Chapter 7.
 *******************************************************************
 */

#include <stdio.h>
#include <math.h>
#include "JD.h"

int is_gregorian_date (int Y, int M, double D) {
    return ((Y>1582)||((Y==1582)&&(M>10))||((Y==1582)&&(M==10)&&(D>=15.0)));
}

int is_julian_date (int Y, int M, double D) {
    return ((Y<1582)||((Y==1582)&&(M<10))||((Y==1582)&&(M==10)&&(D<5.0)));
}

/* The algorithm for this function is described on page 60 of ref(1). */
int Calendar_Date_to_JD (int Y, int M, double D, double * JD) {

  int A, B;

  /* Is the day valid? */
  if ((D < 1.0) || (M >= 32.0)) {
      printf("%s: Day Invalid.", __FUNCTION__);
      return -1;
  }

  /* Is the month valid? */
  if ((M < 1) || (M > 12)) {
      printf("%s: Month Invalid.", __FUNCTION__);
      return -1;
  }

  /* Is the year valid? */
  if ((Y < -4712) || ((Y == -4712) && (D < 1.5))) {
      printf("%s: Year Invalid.", __FUNCTION__);
      return -1;
  }

  if (M <= 2) {
    Y = Y - 1;
    M = M + 12;
  }

  A = (int)(Y / 100.0);

  if (is_gregorian_date(Y,M,D)) {
    B = 2 - A + (int)(A/4.0);
  } else if (is_julian_date (Y,M,D)) {
    B = 0;
  } else {
    return -1;
  }

  *JD = (int)(365.25 * (Y + 4716)) + (int)(30.6001 * (M+1)) + D + B - 1524.5;

  return 0;
}

/* The algorithm for this function is described on page 63 of ref(1). */
void JD_to_Calendar_Date (double JD, int *Year, int *Month, double *Day) {

     int Z,A,B,C,D,E;
     int alpha, year, month;
     double day;
     double F;

     Z = (int)(JD + 0.5);
     F = (JD + 0.5) - Z;
     if ( Z < 2299161 ) {
             A = Z;
     } else {
             alpha = (int)( (Z - 1867216.25) / 36524.25 );
             A = Z + 1 + alpha - (int)(alpha/4);
     }
     B = A + 1524;
     C = (int)((B-122.1)/365.25);
     D = (int)(365.25 * C);
     E = (int)((B-D)/30.6001);

     day = B - D - (int)(30.6001 * E) + F;

     if ( E < 14) {
          month = E - 1;
     } else {
          month = E - 13;
     }

     if ( month > 2 ) {
          year = C -4716;
     } else {
          year = C -4715;
     }

     *Day   = day;
     *Month = month;
     *Year  = year;
}

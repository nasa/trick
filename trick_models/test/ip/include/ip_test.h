/* TRICK HEADER
PURPOSE:
    (Test all possible data types recognized by the Interface Code
     Generator and the executive input processor.)

REFERENCE:
    (((Bailey, Robert W.)
      (User's Guide and Operational Procedures Volume ...
       of the Trick Simulation Environment) (MDSS-HD TM-6.24.26-04)
      (McDonnell Douglas Space Systems - Houston Division) (March 1993) (--)))

ASSUMPTIONS AND LIMITATIONS:
    ((Unlimited software array dimensions and sizing)
     (Unlimited software dynamically allocated array dimensions and sizing)
     (Excessively large structures may overrun hardware memory)
     (All parameter declarations MUST start in the first column))

PROGRAMMERS:
    (((Robert W. Bailey) (LinCom) (10/1/90) (Trick-CR-00000) (Initial Release)))
*/

#ifndef IP_TEST_H
#define IP_TEST_H

/*=== THESE DEFINES ARE PARSED AND USABLE IN ARRAY DIMENSIONS ===*/
#define TWO 2         /* THE Value 2 */
#define TWO2 (1+1)    /* THE Value 2 */
#define TWO3 2*1

/*=== THESE DEFINES ARE IGNORED ===*/
#define DUMMY_MACRO1(A,B) A = B + 1
#define DUMMY_MACRO2(A,B) { \
A = B + 1 ; \
B++ ; \
}

/*==== SYSTEM INCLUDES ARE IGNORED ===*/
#include <stdio.h>    /* ignored */
#include <sys/time.h>   /* ignored */

/*==== USER INCLUDES ARE PARSED AND STORED FOR FUTURE REFERENCE ====*/
#include "ip_c_types.h"

/*==== ALL GLOBAL DECLARATIONS ARE IGNORED ====*/
int * int_function() ;                                 /* ignored */
float real_function( int bogus1 , double *bogus2[] ) ; /* ignored */

/*==== THESE TYPEDEFS ARE IGNORED ===*/
typedef unsigned int Int_Type ; /* ignored */
typedef double* Double_Ptr ;
typedef void (*Void_Func)() ;   /* ignored */

typedef enum {      /* ENUMERATED TYPES --------------------------------------*/

   meter = -2 ,    /* Meters */
   centimeter ,    /* CentiMeters */
   millimeter ,    /* MilliMeters */
   kilometer ,     /* KiloMeters */
   inches = 10 ,   /* Inches */
   feet ,          /* Feet */
   yard ,          /* Yards */
   mile = 5 ,      /* Miles */
   nautical_mile   /* Nautical Miles */

} Distance ; /*---------------------------------------------------------------*/

typedef struct {     /* MEASUREMENT UNITS_TEST SPECIFICATION -----------------*/

   double second[5] ;       /* s     Measurement units */
   double meter[10] ;       /* M     Measurement units */
   double radian[5] ;       /* r     Measurement units */
   double kilogram[6] ;     /* kg    Measurement units */
   double newton[5] ;       /* N     Measurement units */
   double volt[3] ;         /* v     Measurement units */
   double amp[3] ;	    /* amp   Measurement units */
   double ohm[2] ;     	    /* ohm   Measurement units */
   double celsius[4] ;      /* C     Measurement units */
   double fahrenheit[4] ;   /* F     Measurement units */
   double temp_rate[4] ;    /* K/s   Measurement units */
   double area[3] ;         /* M2    Measurement units */
   double volume[1+1+1] ;   /* M3    Measurement units */
   double velocity[4-1] ;   /* M/s   Measurement units */
   double velocity_eng[2] ; /* mi/hr Measurement units using non-std units */
   double accel[TWO+1] ;    /* M/s2  Measurement units */
   double inertia[3*1] ;    /* kg*M2 Measurement units */
   double torque[TWO*2-1] ; /* N*M   Measurement units */
   double pressure[3] ;     /* N/M2  Measurement units */
   double density[3] ;      /* kg/M3 Measurement units */
   Distance measure ;       /* --    Distance measurement units */

} UNITS_TEST ; /*-------------------------------------------------------------*/


typedef struct {     /* COMPOSITE DATA STRUCTURES ----------------------------*/

BITS         bits[TWO] ;           /* -- Two bits, four bits, six bits, a $ */
C_TYPES      c_types ;             /* -- Standard C types */
C_ARR_TYPES  c_array_types ;       /* -- Arrayed C types */
C_PTR_TYPES  c_pointer_types ;     /* -- Pointered C types */
C_MIX_TYPES  c_mixed_types ;       /* -- Mixed arrays and pointer types */
UNITS_TEST **units_test[TWO] ;     /* -- Measurement unit test params */
int          strings_good ;        /* -- 1-good 0-bad */

} IP_TEST ; /*----------------------------------------------------------------*/

#endif

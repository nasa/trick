/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Data products test )
REFERENCES:                  ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

#ifndef DP_H
#define DP_H

typedef struct {

   short          si[3] ;   /*  ohm -- */
   int             i[3] ;   /*  m -- */
   long            l[3] ;   /*  amp -- */
   float           f[3] ;   /*  m/s2 -- */
   double          d[3] ;   /*  kg*m/s2 -- */
   unsigned short us[3] ;   /*  N -- */
   unsigned int   ui[3] ;   /*  count -- */
   unsigned long  ul[3] ;   /*  V -- */
   long long      ll[3] ;   /*  K -- */

} STRUCT_1 ;

typedef struct {

   STRUCT_1 s1[2][3] ;      /* -- */

} STRUCT_2 ;

typedef struct {

   STRUCT_2 s2[3][2] ;      /* -- */

} STRUCT_3 ;

typedef struct {

  STRUCT_3 s3[2]  ;         /* -- */

} DP_TEST ;


#endif

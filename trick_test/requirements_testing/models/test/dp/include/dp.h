/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Data products test ) 
REFERENCES:                  ( None )
ASSUMPTIONS AND LIMITATIONS: ( None ) 
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

/*
 * $Log: dp.h,v $
 * Revision 7.2  2007-01-08 13:55:09-06  dstrauss
 * Bugs in parsing new units specs
 *
 * Revision 7.1  2006-06-22 14:14:18-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:52-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:23  lin
 * Bump version number for 04
 *
 * Revision 1.2  2002/10/07 15:16:22  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#ifndef _DATA_PRODUCTS_H_
#define _DATA_PRODUCTS_H_

typedef struct {

   short          si[3] ;   /*  ohm -- */ 
   int             i[3] ;   /*  M -- */
   long            l[3] ;   /*  amp -- */
   float           f[3] ;   /*  M/s2 -- */
   double          d[3] ;   /*  kg*M/s2 -- */
   unsigned short us[3] ;   /*  N -- */
   unsigned int   ui[3] ;   /*  dB -- */
   unsigned long  ul[3] ;   /*  v -- */
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

/********************************* TRICK HEADER *******************************
PURPOSE:                     ( Test data products )
REFERENCE:                   ( None )
ASSUMPTIONS AND LIMITATIONS: ( None )
CLASS:                       ( scheduled )
LIBRARY DEPENDENCY:          ( dp.o )
PROGRAMMERS:                 ( (Keith Vetter) (Titan) (8-20-2002) )
*******************************************************************************/

#include "../include/dp.h"


/*
 * Loads 1,2,3 into the last dim on -all- vars in STRUCT_1
 */
int dp(
                  /* RETURN: -- Always return zero */
  DP_TEST *DP )      /* INOUT:  -- Schedule struct */
{


#define LOOP for ( i = 0 ; i < 3 ; i++ ) \
             for ( j = 0 ; j < 3 ; j++ ) \
             for ( k = 0 ; k < 2 ; k++ ) \
             for ( l = 0 ; l < 2 ; l++ ) \
             for ( m = 0 ; m < 3 ; m++ ) \
             for ( n = 0 ; n < 2 ; n++ )

   int i,j,k,l,m,n ;

   LOOP DP->s3[n].s2[m][l].s1[k][j].si[i]  = (short)          (i+1) ;
   LOOP DP->s3[n].s2[m][l].s1[k][j].i[i]   = (int)            (i+1) ;
   LOOP DP->s3[n].s2[m][l].s1[k][j].l[i]   = (long)           (i+1) ;
   LOOP DP->s3[n].s2[m][l].s1[k][j].f[i]   = (float)          (i+1) ;
   LOOP DP->s3[n].s2[m][l].s1[k][j].d[i]   = (double)         (i+1) ;
   LOOP DP->s3[n].s2[m][l].s1[k][j].us[i]  = (unsigned short) (i+1) ;
   LOOP DP->s3[n].s2[m][l].s1[k][j].ui[i]  = (unsigned int)   (i+1) ;
   LOOP DP->s3[n].s2[m][l].s1[k][j].ul[i]  = (unsigned long)  (i+1) ;
   LOOP DP->s3[n].s2[m][l].s1[k][j].ll[i]  = (long long)      (i+1) ;

   /* LOOP printf("\"dp.test.s3[%d].s2[%d][%d].s1[%d][%d].d[%d]\" ,\n",
                   n,m,l,k,j,i); */


   return( 0 );
}

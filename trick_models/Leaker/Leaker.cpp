/*
  * $Id:$
 */
#include <vector>
#include <stdlib.h>
#include <string.h>

/* Model include files. */
#include "Ball++/L1/include/Ball.hh"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"

std::vector< int > dv ;

     /* ENTRY POINT */
int Leaker() {

   int * i_malloc = (int *)malloc( sizeof(int) ) ;
   Ball * b_malloc = (Ball *)malloc( sizeof(Ball) ) ;

   int * i_calloc = (int *)calloc( 1 , sizeof(int) ) ;
   Ball * b_calloc = (Ball *)calloc( 1 , sizeof(Ball) ) ;

#if 0
   int * i_realloc = NULL ;
   Ball * b_realloc = NULL ;
   i_realloc = (int *)realloc( i_realloc, sizeof(int) ) ;
   b_realloc = (Ball *)realloc( b_realloc, sizeof(Ball) ) ;
#endif

   int * i_new = new int ;
   Ball * b_new = new Ball ;

   int * ia_new = new int[10] ;
   Ball * ba_new = new Ball[10] ;

   char * str = strdup("hello") ;

   Ball * b_tmms = (Ball *)TMM_declare_var_s("Ball") ;
   Ball * b_tmm1d = (Ball *)TMM_declare_var_1d("Ball", 2) ;
   Ball * b_tmmal = (Ball *)alloc_type(2, "Ball") ;

   int ii ;
   for ( ii = 0 ; ii < 100 ; ii++ ) {
       dv.push_back(ii) ;
   }

   /* RETURN */
   return( 0 );

}


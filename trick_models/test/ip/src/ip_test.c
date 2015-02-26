/*
PURPOSE:
    (Test the Trick simulation executive 'scheduled' module handling.)
REFERENCE:
    (((Bailey, R.W, and Paddock, E.J.)
      (Trick Simulation Environment) (NASA:JSC #37943)
      (JSC / Engineering Directorate / Automation and Robotics Division)
      (Mar 1996) (--)))
ASSUMPTIONS AND LIMITATIONS:
    ((None))
CLASS:
    (scheduled)
LIBRARY DEPENDENCY:
    (ip_test.o)
PROGRAMMERS:
    (((Robert W. Bailey) (LinCom Corp) (9/1/90) (Trick-CR-00000)
        (Initial Release.)))
*/

/*
 * $Log: ip_test.c,v $
 * Revision 5.2  2005-01-31 15:45:54-06  lin
 * remove "register" keyword
 *
 * Revision 5.1  2004-08-05 13:06:55-05  lin
 * Bump
 *
 * Revision 4.1  2003/10/21 21:49:19  lin
 * Bump version number for 04
 *
 * Revision 1.2  2002/10/07 15:16:28  lin
 * Add rcs version info to all trick_models files
 *
 */
 
#include <stdio.h>
#include <stdlib.h>
#include "../include/ip_test.h"

int ip_test(
                       /* RETURN: --    0 - Nominal */
IP_TEST * it )    /* INOUT: -- Executive Input/Output test parameters */

{

char test_string[128] ;
char* trick_home ;

    /* POINTER SHORTHAND ACCESS */
C_TYPES *        C = &(it->c_types) ;
C_ARR_TYPES *    CA = &(it->c_array_types) ;
C_PTR_TYPES *    CP = &(it->c_pointer_types) ;
C_MIX_TYPES *    CM = &(it->c_mixed_types) ;

C->i += 1 ;
C->d += 1.0 ;

CA->sa[0][0][1] += 1 ;
CA->fa[0][1][0] += 1.0 ;

CP->ipp[0][1][0] += 1 ;
CP->dpp[1][0][1] += 1 ;

CM->lpa[1][0][0] += 1 ;
CM->dpa[1][1][0] += 1 ;


trick_home = getenv("TRICK_HOME");

/* Characters */
sprintf( test_string,"%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
      CA->ca[0][0][0],
      CA->ca[0][0][1],
      CA->ca[0][1][0],
      CA->ca[0][1][1],
      CA->ca[1][0][0],
      CA->ca[1][0][1],
      CA->ca[1][1][0],
      CA->ca[1][1][1],

      CA->uca[0][0][0],
      CA->uca[0][0][1],
      CA->uca[0][1][0],
      CA->uca[0][1][1],
      CA->uca[1][0][0],
      CA->uca[1][0][1],
      CA->uca[1][1][0],
      CA->uca[1][1][1],
     
      CM->ucpa[0][0][0], 
      CM->ucpa[0][0][1], 
      CM->ucpa[0][1][0], 
      CM->ucpa[0][1][1], 
      CM->ucpa[1][0][0], 
      CM->ucpa[1][0][1], 
      CM->ucpa[1][1][0], 
      CM->ucpa[1][1][1] ) ;
if ( strcmp( test_string, "abcdefghijklmnopABCDEFGH" ) ) {
  fprintf(stderr, "Error_characters: %s \n", test_string);
  it->strings_good = 0 ;
}

/* The regular string */
if ( strcmp( C->cp, "Testing Char Pointers.") ) { 
  if ( strcmp( C->cp, trick_home ) ) {
     fprintf(stderr, "Error_character_ptr: %s \n", C->cp);
     it->strings_good = 0 ;
  }
}

/*
 * The character pointers
 */
if ( strcmp( CP->cpp[0][0][0], "I'd" ) ) {
   if ( strcmp( CP->cpp[0][0][0], trick_home ) ) {
      if ( strcmp(CP->cpp[0][0][0], "Cat") ) {
         fprintf(stderr, "Error_p1: %s \n", CP->cpp[0][0][0]);
         it->strings_good = 0 ;
      }
   }
}
if ( strcmp( CP->cpp[0][0][1], "rather" ) ) {
   if ( strcmp( CP->cpp[0][0][1], trick_home ) ) {
      if ( strcmp(CP->cpp[0][0][1], "Horse") ) {
         fprintf(stderr, "Error_p2: %s \n", CP->cpp[0][0][1]);
         it->strings_good = 0 ;
      }
   }
}
if ( strcmp( CP->cpp[0][1][0], "be" ) ) {
   if ( strcmp( CP->cpp[0][1][0], trick_home ) ) {
      fprintf(stderr, "Error_p3: %s \n", CP->cpp[0][1][0]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CP->cpp[0][1][1], "developing" ) ) {
   if ( strcmp( CP->cpp[0][1][1], trick_home ) ) {
      fprintf(stderr, "Error_p4: %s \n", CP->cpp[0][1][1]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CP->cpp[1][0][0], "than" ) ) {
   if ( strcmp( CP->cpp[1][0][0], trick_home ) ) {
      fprintf(stderr, "Error_p5: %s \n", CP->cpp[1][0][0]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CP->cpp[1][0][1], "testing" ) ) {
   if ( strcmp( CP->cpp[1][0][1], trick_home ) ) {
      fprintf(stderr, "Error_p6: %s \n", CP->cpp[1][0][1]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CP->cpp[1][1][0], "or" ) ) {
   if ( strcmp( CP->cpp[1][1][0], trick_home ) ) {
      fprintf(stderr, "Error_p7: %s \n", CP->cpp[1][1][0]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CP->cpp[1][1][1], "debugging." ) ) {
   if ( strcmp( CP->cpp[1][1][1], trick_home ) ) {
      fprintf(stderr, "Error_p8: %s \n", CP->cpp[1][1][1]);
      it->strings_good = 0 ;
   }
}

/*
 * The character arrays
 */
if ( strcmp( CM->cpa[0][0][0], "I" ) ) {
   if ( strcmp( CP->cpp[1][0][1], trick_home ) ) {
      fprintf(stderr, "Error_a1: %s \n", CM->cpa[0][0][0]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CM->cpa[0][0][1], "would" ) ) {
   if ( strcmp( CP->cpp[1][0][1], trick_home ) ) {
      fprintf(stderr, "Error_a2: %s \n", CM->cpa[0][0][1]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CM->cpa[0][1][0], "rather" ) ) {
   if ( strcmp( CP->cpp[1][0][1], trick_home ) ) {
      fprintf(stderr, "Error_a3: %s \n", CM->cpa[0][1][0]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CM->cpa[0][1][1], "be" ) ) {
   if ( strcmp( CP->cpp[1][0][1], trick_home ) ) {
      fprintf(stderr, "Error_a4: %s \n", CM->cpa[0][1][1]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CM->cpa[1][0][0], "surfing" ) ) {
   if ( strcmp( CP->cpp[1][0][1], trick_home ) ) {
      fprintf(stderr, "Error_a5: %s \n", CM->cpa[1][0][0]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CM->cpa[1][0][1], "today" ) ) {
   if ( strcmp( CP->cpp[1][0][1], trick_home ) ) {
      fprintf(stderr, "Error_a6: %s \n", CM->cpa[1][0][1]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CM->cpa[1][1][0], "in" ) ) {
   if ( strcmp( CP->cpp[1][0][1], trick_home ) ) {
      fprintf(stderr, "Error_a7: %s \n", CM->cpa[1][1][0]);
      it->strings_good = 0 ;
   }
}
if ( strcmp( CM->cpa[1][1][1], "the blue oceans of Wakiki." ) ) {
   if ( strcmp( CP->cpp[1][0][1], trick_home ) ) {
     fprintf(stderr, "Error_a8: %s \n", CM->cpa[1][1][1]);
     it->strings_good = 0 ;
   }
}

return( 0 ) ;
}

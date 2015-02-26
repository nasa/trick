
/*********************************** TRICK HEADER **************************
PURPOSE:                     (Test ip)
REFERENCES:                  (none)
ASSUMPTIONS AND LIMITATIONS: (Autocreated by trick_ui)
CLASS:                       (initialization)
LIBRARY DEPENDENCY:          (ip.o)
PROGRAMMERS:                 ((lin) (Thu Feb 27 14:57:16 CST 2003))
***************************************************************************/

/*
 * $Log: ip_test_init.c,v $
 * Revision 7.2  2006-08-15 15:54:44-05  hchen
 * SIM_ip2_test needs to be updated for new ip_alloc_type interface
 *
 * Revision 7.1  2006-06-22 14:14:12-05  lin
 * Bump version number for 07
 *
 * Revision 5.1  2004-08-05 13:06:56-05  lin
 * Bump
 *
 * Revision 4.2  2004/01/16 21:48:55  lin
 * Add a log to all files
 *
 */

#include <stdio.h>
#include "../include/ip.h"
#include "sim_services/include/attributes.h" 
#include "sim_services/include/exec_proto.h" 

extern ATTRIBUTES attrstruct_LinkedList[] ;

int ip_test_init(
                        /* RETURN: -- Always return zero */
  LINKED_LIST** L)       /* INOUT:  -- Parameter */

{

   LINKED_LIST * new_item ;

   new_item = (LINKED_LIST *)ip_alloc_type( 1 , sizeof(LINKED_LIST), attrstruct_LinkedList, NULL ) ;
   new_item->i = 1000 ;

   *L = new_item ;

   new_item = (LINKED_LIST *)ip_alloc_type( 1 , sizeof(LINKED_LIST), attrstruct_LinkedList, NULL ) ;
   new_item->i = 2000 ;

   new_item->prev = *L ;
   (*L)->next = new_item ;

   new_item = (LINKED_LIST *)ip_alloc_type( 2 , sizeof(LINKED_LIST), attrstruct_LinkedList, NULL ) ;
   new_item->i = 500 ;
   new_item[1].i = 501 ;

   new_item = (LINKED_LIST *)ip_realloc( (char *)new_item , 8 , sizeof(LINKED_LIST)) ;
   new_item->prev = *L ;
   (*L)->next->next = new_item ;
   new_item[2].i = 502 ;
   new_item[3].i = 503 ;
   new_item[4].i = 504 ;
   new_item[5].i = 505 ;
   new_item[6].i = 506 ;

   /*
   new_item = (LINKED_LIST *)ip_alloc( 1 , sizeof(LINKED_LIST), attrLINKED_LIST ) ;
   new_item->i = 500 ;

   (*L)->prev = new_item ;

   new_item = (LINKED_LIST *)ip_alloc( 1 , sizeof(LINKED_LIST), attrLINKED_LIST ) ;
   new_item->i = 4000 ;

   (*L)->next->next = new_item ;
   */

   return ( 0 ); 
}

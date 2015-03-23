
/*********************************** TRICK HEADER **************************
PURPOSE:                     (Test ip)
REFERENCES:                  (none)
ASSUMPTIONS AND LIMITATIONS: (Autocreated by trick_ui)
CLASS:                       (initialization)
LIBRARY DEPENDENCY:          (ip.o)
PROGRAMMERS:                 ((lin) (Thu Feb 27 14:57:16 CST 2003))
***************************************************************************/

#include <stdio.h>
#include "../include/ip.h"
#include "sim_services/MemoryManager/include/attributes.h"
#include "sim_services/Executive/include/exec_proto.h"
#include "sim_services/MemoryManager/include/memorymanager_c_intf.h"

extern ATTRIBUTES attrstruct_LinkedList[] ;

int ip_test_init(           /* RETURN: -- Always return zero */
        LINKED_LIST** L)    /* INOUT:  -- Parameter */

{

    LINKED_LIST * new_item ;

    /* Allocate memory for a LINKED_LIST struct. */
    new_item = (LINKED_LIST *)TMM_declare_var_s( "LINKED_LIST" ) ;
    new_item->i = 1000 ;

    *L = new_item ;

    /* Allocate memory for an array of 1 LINKED_LIST struct. */
    /* Same as above, but can be resized if desired. */
    new_item = (LINKED_LIST *)TMM_declare_var_s( "LINKED_LIST[1]" ) ;
    new_item->i = 2000 ;

    new_item->prev = *L ;
    (*L)->next = new_item ;

    /* Allocate memory for an array of LINKED_LIST structures. */
    new_item = (LINKED_LIST *)TMM_declare_var_s( "LINKED_LIST[2]" ) ;
    new_item->i = 500 ;
    new_item[1].i = 501 ;

    /* Resize one-dimensional array if given an address and new size.  */
    /* Note: Memory reallocation is only possible for arrays.  */
    new_item = TMM_resize_array_1d_a( new_item, 8 ) ;
    new_item->prev = *L ;
    (*L)->next->next = new_item ;
    new_item[2].i = 502 ;
    new_item[3].i = 503 ;
    new_item[4].i = 504 ;
    new_item[5].i = 505 ;
    new_item[6].i = 506 ;

    /*
    new_item = (LINKED_LIST *)ip_alloc( 1 , sizeof(LINKED_LIST), attrLINKED_LIST ) ;
    new_item = (LINKED_LIST *)TMM_declare_var_s( "LINKED_LIST" ) ;
    new_item->i = 500 ;

    (*L)->prev = new_item ;

    new_item = (LINKED_LIST *)ip_alloc( 1 , sizeof(LINKED_LIST), attrLINKED_LIST ) ;
    new_item = (LINKED_LIST *)TMM_declare_var_s( "LINKED_LIST" ) ;
    new_item->i = 4000 ;

    (*L)->next->next = new_item ;
    */

    return ( 0 );
}

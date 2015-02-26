/*******************************************************************************
*                                                                              *
* Trick Simulation Environment Software                                        *
*                                                                              *
* Copyright (c) 1996,1997 LinCom Corporation, Houston, TX                      *
* All rights reserved.                                                         *
*                                                                              *
* Copyrighted by LinCom Corporation and proprietary to it. Any unauthorized    *
* use of Trick Software including source code, object code or executables is   *
* strictly prohibited and LinCom assumes no liability for such actions or      *
* results thereof.                                                             *
*                                                                              *
* Trick Software has been developed under NASA Government Contracts and        *
* access to it may be granted for Government work by the following contact:    *
*                                                                              *
* Contact: Charles Gott, Branch Chief                                          *
*          Simulation and Graphics Branch                                      *
*          Automation, Robotics, & Simulation Division                         *
*          NASA, Johnson Space Center, Houston, TX                             *
*                                                                              *
*******************************************************************************/
/* 
   PURPOSE: (Construct a parse tree representation of a parameter reference name from the input file.)

   PROGRAMMERS: (((Robert W. Bailey) (LinCom) (7/95) (CR#600) (--)) ((Robert W. Bailey) (LinCom Corp) (Feb 1991) (v1.0) 
   (Initial Release.))) */

/* 
 * $Id: MemoryManager_ref_name.cpp 3556 2014-06-17 15:58:09Z alin $
 */

#include <string.h>

#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/MemoryManager/include/attributes.h"
#include "sim_services/MemoryManager/include/reference.h"
#include "sim_services/MemoryManager/include/parameter_types.h"
#include "sim_services/MemoryManager/include/mm_error.h"
#include "sim_services/MemoryManager/include/message_proto.h"

//FIXME TODO make a error file
/////// MM_NO_ERROR 0
#define MM_PARAMETER_NAME 1 
#define MM_INPUT_NOT_ALLOWED 2


int Trick::MemoryManager::ref_name(REF2 * R, char *name) {                      

    int ii;
    char *addr;
    ATTRIBUTES *attr;

    attr = (ATTRIBUTES*)(R->attr->attr);

    if (attr == NULL) {
        return (MM_PARAMETER_NAME);
    }

    if (R->address == NULL) {
        message_publish(MSG_ERROR, "Memory Manager ERROR: Since the value of the pointer \"%s\" is NULL,\n"
                                   "    a legitimate address can't be calculated for \"%s.%s\" in ref_name.\n",
                        R->reference, R->reference, name);
        return (MM_PARAMETER_NAME);
    }

    /* Find the parameter name at this level in the parameter list, 'ii' is the index to the parameter in the list. */
    ii = 0;
    while (strcmp(name, attr[ii].name)) {
        if (attr[ii].name[0] == '\0')
            return (MM_PARAMETER_NAME);
        ii++;
    }

    attr = &(attr[ii]);

//    R->deprecated |= (attr->mods & 0x80000000);

/* Set error_attr just in case we have an error */
//    R->error_attr = attr;

    /* If the variable is a static var, offset is the absolute address */
    if (attr->mods & 2) {
        addr = (char *) attr->offset;
        if ( R->create_add_path ) {
            DLLPOS list_pos ;
            ADDRESS_NODE * address_node ;
            list_pos = DLL_GetHeadPosition(R->address_path) ;
            while ( (list_pos != NULL) && (address_node = (ADDRESS_NODE *)DLL_GetNext(&list_pos, R->address_path)) != NULL ) {
                if ( (list_pos != NULL) && (list_pos->info != NULL) ) {
                    delete (ADDRESS_NODE *)(list_pos->info) ;
                }
            }
            DLL_RemoveAll(R->address_path) ;
            address_node = new ADDRESS_NODE ;
            address_node->operator_ = AO_ADDRESS ;
            address_node->operand.address = addr ;
            DLL_AddTail(address_node , R->address_path) ;
        }
    } else {
        addr = (char *)R->address + attr->offset;
        if ( R->create_add_path ) { 
            ADDRESS_NODE * address_node ;

            if ( attr->offset != 0 ) {
                address_node = (ADDRESS_NODE *)DLL_GetAt(DLL_GetTailPosition(R->address_path), R->address_path) ;
                switch ( address_node->operator_ ) {
                    case AO_ADDRESS:
                        address_node->operand.address = (void *)((char *)address_node->operand.address +  attr->offset) ;
                        break ;
                    case AO_DEREFERENCE:
                        address_node = new ADDRESS_NODE ;
                        address_node->operator_ = AO_OFFSET ;
                        address_node->operand.offset = attr->offset ;
                        DLL_AddTail(address_node , R->address_path) ;
                        break ;
                    case AO_OFFSET:
                        address_node->operand.offset += attr->offset ;
                        break ;
                }
            }
        }
    }

    /* Save the address and next attributes in the REF structure */
    R->attr = attr;
    R->address = addr;

    return (MM_OK);
}

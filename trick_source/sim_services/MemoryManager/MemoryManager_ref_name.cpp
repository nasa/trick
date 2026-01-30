/*
   PURPOSE: (Construct a parse tree representation of a parameter reference name from the input file.)

   PROGRAMMERS: (((Robert W. Bailey) (LinCom) (7/95) (CR#600) (--)) ((Robert W. Bailey) (LinCom Corp) (Feb 1991) (v1.0)
   (Initial Release.))) */

#include <sstream>
#include <string.h>
#include <string>

#include "trick/MemoryManager.hh"
#include "trick/attributes.h"
#include "trick/mm_error.h"
#include "trick/parameter_types.h"
#include "trick/reference.h"
#include <stdlib.h>

// FIXME TODO make a error file
/////// MM_NO_ERROR 0
#define MM_PARAMETER_NAME 1
#define MM_INPUT_NOT_ALLOWED 2

// Helper function to trim whitespace from both ends of a string
static std::string trim(const std::string & str)
{
    size_t start = str.find_first_not_of(" \t\n\r");
    if(start == std::string::npos)
    {
        return "";
    }

    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

int Trick::MemoryManager::ref_name(REF2 * R, char * name)
{
    int ii;
    char * addr;
    ATTRIBUTES * attr;

    attr = (ATTRIBUTES *)(R->attr->attr);

    if(attr == NULL)
    {
        // Special handling for STL containers with structured elements
        // ICG doesn't populate attr field for these, so we look it up at runtime
        if(R->attr->type == TRICK_STL && R->attr->stl_elem_type == TRICK_STRUCTURED &&
           R->attr->stl_elem_type_name != NULL)
        {
            // Get element type name directly from stl_elem_type_name field
            std::string elem_type_name = R->attr->stl_elem_type_name;

            // Check if the element type is a pointer (contains '*')
            bool is_pointer = (elem_type_name.find('*') != std::string::npos);

            // If it's a pointer, strip the pointer symbols and whitespace to get the base type
            if(is_pointer)
            {
                size_t star_pos = elem_type_name.find('*');
                elem_type_name = trim(elem_type_name.substr(0, star_pos));
            }

            // Look up the element type's attributes without modifying the static structure
            ATTRIBUTES temp_attr;
            memset(&temp_attr, 0, sizeof(ATTRIBUTES));

            if(add_attr_info(elem_type_name, &temp_attr) == 0 && temp_attr.attr != NULL)
            {
                attr = (ATTRIBUTES *)temp_attr.attr;

                // If element is a pointer type, we need to dereference it first
                if(is_pointer)
                {
                    if(R->address == NULL)
                    {
                        std::stringstream message;
                        message << "ref_name: Address is NULL for pointer element in STL container \"" << R->reference
                                << "\".";
                        emitError(message.str());
                        return (MM_PARAMETER_NAME);
                    }
                    // Dereference the pointer to get to the actual object
                    R->address = *(void **)R->address;
                    if(R->address == NULL)
                    {
                        std::stringstream message;
                        message << "ref_name: Pointer element is NULL in STL container \"" << R->reference << "\".";
                        emitError(message.str());
                        return (MM_PARAMETER_NAME);
                    }
                }
            }
            else
            {
                return (MM_PARAMETER_NAME);
            }
        }
        else
        {
            return (MM_PARAMETER_NAME);
        }
    }

    if(R->address == NULL)
    {
        std::stringstream message;
        message << "ref_name: Because the address of \"" << R->reference
                << "\" is NULL, a legitimate address can't be calculated for \"" << R->reference << "." << name
                << "\".";
        emitError(message.str());
        return (MM_PARAMETER_NAME);
    }

    /* Find the parameter name at this level in the parameter list, 'ii' is the index to the parameter in the list. */
    ii = 0;
    while(strcmp(name, attr[ii].name))
    {
        if(attr[ii].name[0] == '\0')
        {
            return (MM_PARAMETER_NAME);
        }
        ii++;
    }

    attr = &(attr[ii]);

    //    R->deprecated |= (attr->mods & 0x80000000);

    /* Set error_attr just in case we have an error */
    //    R->error_attr = attr;

    /* If the variable is a static var, offset is the absolute address */
    if(attr->mods & 2)
    {
        addr = (char *)attr->offset;
        if(R->create_add_path)
        {
            DLLPOS list_pos;
            ADDRESS_NODE * address_node;
            list_pos = DLL_GetHeadPosition(R->address_path);
            while((list_pos != NULL) &&
                  (address_node = (ADDRESS_NODE *)DLL_GetNext(&list_pos, R->address_path)) != NULL)
            {
                if((list_pos != NULL) && (list_pos->info != NULL))
                {
                    delete(ADDRESS_NODE *)(list_pos->info);
                }
            }
            DLL_RemoveAll(R->address_path);
            address_node = new ADDRESS_NODE;
            address_node->operator_ = AO_ADDRESS;
            address_node->operand.address = addr;
            DLL_AddTail(address_node, R->address_path);
        }
    }
    else
    {
        addr = (char *)R->address + attr->offset;
        if(R->create_add_path)
        {
            ADDRESS_NODE * address_node;

            if(attr->offset != 0)
            {
                address_node = (ADDRESS_NODE *)DLL_GetAt(DLL_GetTailPosition(R->address_path), R->address_path);
                switch(address_node->operator_)
                {
                    case AO_ADDRESS:
                        address_node->operand.address = (void *)((char *)address_node->operand.address + attr->offset);
                        break;
                    case AO_DEREFERENCE:
                        address_node = new ADDRESS_NODE;
                        address_node->operator_ = AO_OFFSET;
                        address_node->operand.offset = attr->offset;
                        DLL_AddTail(address_node, R->address_path);
                        break;
                    case AO_OFFSET:
                        address_node->operand.offset += attr->offset;
                        break;
                }
            }
        }
    }

    if(attr->mods & 1)
    {
        ADDRESS_NODE * address_node = new ADDRESS_NODE;
        address_node->operator_ = AO_DEREFERENCE;
        address_node->operand.address = NULL;
        DLL_AddTail(address_node, R->address_path);

        addr = *(char **)addr;
    }

    /* Save the address and next attributes in the REF structure.
       If the attributes are dynamically-allocated, reference attributes
       then free them so we don't leak memory.
    */
    if(R->attr == R->ref_attr)
    {
        free(R->ref_attr);
        R->ref_attr = NULL;
    }
    R->attr = attr;
    R->address = addr;

    return (MM_OK);
}

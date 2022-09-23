#ifndef PYTHONPRINT_HH
#define PYTHONPRINT_HH

#include <stddef.h> // for NULL
#include <string>
#include <iostream>
#include <vector>
#include "trick/CheckPointAgent.hh"

namespace Trick {

class PythonPrint: public CheckPointAgent {

    public:

    /**
     Test incoming attributes permission check.
     @param attr Attributes with permision to check.
     */
    virtual bool input_perm_check(ATTRIBUTES * attr) ;

    /**
     Test incoming attributes permission check.
     @param attr Attributes with permision to check.
     */
    virtual bool output_perm_check(ATTRIBUTES * attr) ;

    /**
     Write a declaration that describes the given ALLOC_INFO object.
     */
    virtual void write_decl( std::ostream& chkpnt_os, ALLOC_INFO *alloc_info) ;

    /**
     Write an assignment statement that assigns the specified
     object to the name on the left-side stack.
     */
    virtual void assign_rvalue( std::ostream& chkpnt_os,
                                void*       address,
                                ATTRIBUTES* attr,
                                int         curr_dim,
                                int         offset
                                );
    /**
     Restore Checkpoint.
     */
    virtual int restore( std::istream* checkpoint_stream);

    /**
     Write a single value of an intrinsic type located at the given address and offset.

     @param address - address of the object that contains the value to be written.
     @param attr - ATTRIBUTES of the object that contains the value to be written.
     @param offset - if the object specified by address and attr is arrayed, then this
                     parameter specifies where, within the array the single value is
                     located. @c offset=0 means that the address points to the value
                     to be written.
     */
    static void write_singleton( std::ostream& chkpnt_os, void* address,
     ATTRIBUTES* attr, int offset, bool write_units = true , bool in_list = false );

    /**
     I NEED DOCUMENTATION!
     */
    static void write_rvalue( std::ostream& chkpnt_os, void* address,
     ATTRIBUTES* attr, int curr_dim, int offset, bool write_units = true , bool in_list = false );

    /**
     Convert a pointer into a text expression that represents that pointer and is
     suitable for the right side of an assignment statement.
     The pointer must be an address that is being managed by MM (the MemoryManager) or a
     character string (char* or wchar_t*).

     @param pointer
     @param attr ATTRIBUTES of the pointer.
     @param curr_dim current dimension.
     @return text expression that represents the pointer.
     */
    static std::string ref_string_from_ptr( void* pointer, ATTRIBUTES* attr, int curr_dim);

    protected:

    /**
     @return the name represented by the left-side name stack.
     */
    std::string left_side_name();

};
} //namespace
#endif

#ifndef CLASSICCHECKPOINT_HH
#define CLASSICCHECKPOINT_HH

#include <stddef.h> // for NULL
#include <string>
#include <vector>
#include "trick/CheckPointAgent.hh"

namespace Trick {

    /**
     This class provides checkpointing.
     @author John M. Penn
     */
    class ClassicCheckPointAgent: public CheckPointAgent {

        public:

        static const int array_elements_per_line[TRICK_NUMBER_OF_TYPES]; /**< ** array elements per line  */

        /**
         Constructor.
         @param  MM MemoryManager.
         */
        ClassicCheckPointAgent( Trick::MemoryManager *MM);

        ~ClassicCheckPointAgent();

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
         Write a @ref declaration statement for the given allocation.
         @param alloc_info Pointer to the allocation record.
         */
        void write_decl(std::ostream& chkpnt_os, ALLOC_INFO *alloc_info);

        /**
         In the specified checkpoint file, generate a statement that assigns
         one of the following to the left-side variable:
         - One or more values of an intrinsic data type
         - A pointer

         @param address Address of the variable.
         @param attr ATTRIBUTES of the variable.
         @param curr_dim Dimensions of the array
         @param offset Offset into the array
         */
        void assign_rvalue( std::ostream& chkpnt_os, void* address, ATTRIBUTES* attr, int curr_dim, int offset);

        /**
         Restore memory allocations from a checkpoint stream.
         @param checkpoint_stream Input stream from which the checkpoint is read.
         @return 0/1 success flag
         */
        int restore( std::istream* checkpoint_stream);

        /**
         Confirm that the value of an intrinsic typed sub-element, at the specified offset,
         within the object at the given address, is zero.
         To check the entire object at the address, call with curr_dim=0 and offset=0;

         @param address - address of the object within which we are checking for nil values.
         @param attr - attr describing the object at the address.
         @param curr_dim - dimension of the sub-element being checked.
         @param offset - if the object specified by @b address and @b attr is arrayed, then this
                         parameter specifies where, within the array, the sub-object to be
                         checked, is located.
         @return 0=false, 1=true
         */
        int is_nil_valued(void* address, ATTRIBUTES* attr, int curr_dim, int offset );

        /**
         Convert a pointer into a text expression that represents that pointer and is
         suitable for the right side of an assignment statement.
         The pointer must be an address that is being managed by MM (the MemoryManager) or a
         character string (char* or wchar_t*).

         @param pointer Pointer to be converted.
         @param attr ATTRIBUTES of the pointer.
         @param curr_dim current dimension.
         @return text expression that represents the pointer.
         */
        std::string ref_string_from_ptr( void* pointer, ATTRIBUTES* attr, int curr_dim);

    private:

        Trick::MemoryManager *mem_mgr;                 /**< ** Associated MemoryManager. */

        /** Don't Allow the default constructor to be used. */
        ClassicCheckPointAgent();

        /**
         @return the name represented by the left-side name stack.
         */
        std::string left_side_name();

        /**
         I NEED DOCUMENTATION!
         */
        std::string get_var_name( void*   addr,
                                  ATTRIBUTES* A,
                                  void* struct_addr,
                                  std::string name,
                                  ATTRIBUTES** left_type);

        /**
         Write a single value of an intrinsic type located at the given address and offset.

         @param address - address of the object that contains the value to be written.
         @param attr - ATTRIBUTES of the object that contains the value to be written.
         @param offset - if the object specified by address and attr is arrayed, then this
                         parameter specifies where, within the array the single value is
                         located. @c offset=0 means that the address points to the value
                         to be written.
         */
        void write_singleton( std::ostream& chkpnt_os, void* address, ATTRIBUTES* attr, int offset);

        /**
         I NEED DOCUMENTATION!
         */
        void write_rvalue( std::ostream& chkpnt_os, void* address, ATTRIBUTES* attr, int curr_dim, int offset);

    };
} //namespace
#endif

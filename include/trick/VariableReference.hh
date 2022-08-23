/*************************************************************************
PURPOSE: (A variable server variable reference. Refactor of VariableReference
 based on the VariableServerVariable class previously in the webserver.)
**************************************************************************/
#ifndef VARIABLE_REFERENCE_HH
#define VARIABLE_REFERENCE_HH

#include <time.h>
#include <vector>

#include <iostream>
#include <trick/reference.h>

#define MAX_ARRAY_LENGTH 4096

union cv_converter ;
namespace Trick {
    class VariableReference {

    public:
        friend std::ostream& operator<< (std::ostream& s, const Trick::VariableReference& ref);

        VariableReference(std::string var_name);

        // Special constructor to deal with time
        VariableReference(std::string var_name, double* time);

        ~VariableReference();

        const char* getName() const;
        TRICK_TYPE getType() const;

        // There are 2 different "units" variables used - REF2->units and REF2->attr->units
        // REF2->attr->units should not be changed, it is what the variable is represented in internally
        // REF2->units is the unit that has been requested by the variable server client
        // REF2->units should be null unless var_units or var_add with units is called
        // We'll refer to REF2->attr->units as BaseUnits and REF2->units as RequestedUnits
        // Encapsulate all of this away so that no one else has to think about ref->attr->units vs ref->units
        // ^ TODO: this is not where the above documentation should be. put it somewhere else.
        const char* getBaseUnits() const;
        int setRequestedUnits(std::string new_units);

        // These variables have 2 staging buffers that we can swap between to allow for different copy and write-out modes
        // stageValue copies data from the simulation into one of the buffers (stage_buffer)
        // prepareForWrite swaps the pointers for stage_buffer and write_buffer
        // This way we can have data ready for writing, but also be copying out from the sim at the same time
        // stageValue must be called first, and then prepare for write, and then writeValue* can be called.
        int stageValue(bool validate_address = false);
        int prepareForWrite();
        bool isStaged() const;
        bool isWriteReady() const;

        // Write out the value to the given outstream.
        // write_ready must be true
        int getSizeAscii() const;
        int getSizeBinary() const;
        int writeValueAscii( std::ostream& out ) const;
        int writeValueBinary( std::ostream& out , bool byteswap = false) const;
        int writeNameBinary( std::ostream& out, bool byteswap = false) const;
        int writeSizeBinary( std::ostream& out, bool byteswap = false) const;
        int writeTypeBinary( std::ostream& out, bool byteswap = false) const;

        bool validate();
        void tagAsInvalid();

        // Helper method for byteswapping
        static void byteswap_var (char * out, char * in, const VariableReference& ref);

        // TODO: Some system for error messaging

    private:
        VariableReference();
        void byteswap_var(char * out, char * in) const;

        // Error refs
        static REF2* make_error_ref(std::string in_name);
        static REF2* make_do_not_resolve_ref(std::string in_name);

        static int bad_ref_int;
        static int do_not_resolve_bad_ref_int;

        REF2 *var_info;  
        void *address;                      // -- address of data copied to buffer
        int   size;                         // -- size of data copied to buffer
        bool  deref;                        // -- indicates whether variable is pointer that needs to be dereferenced
        cv_converter * conversion_factor ;  // ** udunits conversion factor
        TRICK_TYPE trick_type ;             // -- Trick type of this variable

        // TODO: use atomics for these
        bool staged;
        bool write_ready;

        void *stage_buffer;
        void *write_buffer;  
    };

    std::ostream& operator<< (std::ostream& s, const Trick::VariableReference& ref);
}
#endif

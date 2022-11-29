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
        VariableReference(std::string var_name);
        ~VariableReference();

        const char* getName() const;

        // There are 2 different "units" variables used - REF2->units and REF2->attr->units
        // REF2->attr->units should not be changed, it is what the variable is represented in internally
        // REF2->units is the unit that has been requested by the variable server client
        // REF2->units should be null unless var_units or var_add with units is called
        // Skraddwelkdh needs to name their variables better, this took me so long to figure out >:(
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
        int writeValueAscii( std::ostream& out ) const;
        int writeValueBinary( std::ostream& out ) const;

        bool validate();
        void tagAsInvalid();

        // TODO: Some system for error messaging

    private:
        VariableReference() {}

        static REF2* make_error_ref(std::string in_name);
        static int bad_ref_int;
        static int do_not_resolve_bad_ref_int;

        REF2 *var_info;  
        void *address;                      // -- address of data copied to buffer
        int   size;                         // -- size of data copied to buffer
        bool  deref;                        // -- indicates whether variable is pointer that needs to be dereferenced
        cv_converter * conversion_factor ;  // ** udunits conversion factor
        TRICK_TYPE string_type ;            // -- indicate if this is a string or wstring

        // TODO: use atomics for these
        bool staged;
        bool write_ready;

        void *stage_buffer;
        void *write_buffer;  
    };
}
#endif

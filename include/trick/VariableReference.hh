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
        const char* getName();
        const char* getUnits();
        int setUnits(char * new_units);
        int setConversionFactor(cv_converter * new_conversion_factor);
        void stageValue();
        void prepareForWrite();
        void writeValueAscii( std::ostream& out );
        void writeValueBinary( std::ostream& out );
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
        TRICK_TYPE string_type ;  // -- indicate if this is a string or wstring

        bool need_validate ;
        bool staged;
        bool write_ready;

        void *stage_buffer;
        void *write_buffer;  
    };
}
#endif

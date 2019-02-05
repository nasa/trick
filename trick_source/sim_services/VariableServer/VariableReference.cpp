
#include <stdlib.h>
#include <iostream>
#include <udunits2.h>
#include "trick/VariableServer.hh"
#include "trick/memorymanager_c_intf.h"
#include "trick/wcs_ext.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"

Trick::VariableReference::VariableReference(REF2 * in_ref ) {


    int k ;

    // VariableReference copy setup: set address & size to copy into buffer
    conversion_factor = cv_get_trivial() ;

    ref = in_ref ;
    address = ref->address ;
    size = ref->attr->size ;
    // char* and wchar* in Trick 7 have a type of string and wstring, respectively
    // but in Trick 10 they are type char and wchar (probably John Penn's fault),
    // so we need to keep track that they are really string and wstring
    string_type = ref->attr->type ;
    need_deref = false ;

    if ( ref->num_index == ref->attr->num_index ) {
        // single value
    } else if ( ref->attr->index[ref->attr->num_index - 1].size != 0 ) {
        // fixed array
        for ( k = ref->attr->num_index-1;  k > ref->num_index-1 ; k-- ) {
            size *= ref->attr->index[k].size ;
        }
    } else {
        // arrays with pointers
        if ((ref->attr->num_index - ref->num_index) > 1 ) {
            // you cannot request more than one dimension because they are not contiguous
            message_publish(MSG_ERROR, "Variable Server Error: var_add(%s) requests more than one dimension of dynamic array.\n", ref->reference);
            message_publish(MSG_ERROR, "Data is not contiguous so returned values are unpredictable.\n") ;
        }
        if ( ref->attr->type == TRICK_CHARACTER ) {
        // treat char* like a c++ string (see below)
            string_type = TRICK_STRING ;
            need_deref = true;
        } else if ( ref->attr->type == TRICK_WCHAR ) {
        // treat wchar_t* like a wstring (see below)
            string_type = TRICK_WSTRING ;
        } else {
            need_deref = true ;
            size *= get_size((char*)address) ;
        }
    }

    // handle strings: set a max buffer size, the copy size may vary so will be set in copy_sim_data
    if (( string_type == TRICK_STRING ) || ( string_type == TRICK_WSTRING )) {
        size = MAX_ARRAY_LENGTH ;
    }

    buffer_in  = calloc( size, 1 ) ;
    buffer_out = calloc( size, 1 ) ;


}

std::ostream& Trick::operator<< (std::ostream& s, const Trick::VariableReference& vref) {

    if (vref.ref->reference != NULL) {
        s << "      \"" << vref.ref->reference << "\"";
    } else {
        s<< "      null";
    }
    return s;
}

Trick::VariableReference::~VariableReference() {
    free(ref) ;
    free(buffer_in) ;
    free(buffer_out) ;
}

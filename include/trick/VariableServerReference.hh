/*
    PURPOSE:
        (VariableServerReference)
*/

#ifndef VARIABLESERVERREFERENCE_HH
#define VARIABLESERVERREFERENCE_HH

#include <iostream>
#include "trick/reference.h"

union cv_converter ;

#define MAX_ARRAY_LENGTH 4096

namespace Trick {

/**
  This class provides reference information for variables requested from the variable server by the client.
  @author Alex Lin
 */

    class VariableReference {
        public:
            VariableReference(REF2 * in_ref) ;
            ~VariableReference() ;

            friend std::ostream& operator<< (std::ostream& s, const Trick::VariableReference& vref);

            /** Pointer to trick variable reference structure.\n */
            REF2 * ref ;
            cv_converter * conversion_factor ; // ** udunits conversion factor
            void * buffer_in ;
            void * buffer_out ;
            void * address ;          // -- address of data copied to buffer
            int size ;                // -- size of data copied to buffer
            TRICK_TYPE string_type ;  // -- indicate if this is a string or wstring
            bool need_deref ;         // -- inidicate this is a painter to be dereferenced
    } ;

}

#endif


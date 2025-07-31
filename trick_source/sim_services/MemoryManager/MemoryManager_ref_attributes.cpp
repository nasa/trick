
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include "trick/MemoryManager.hh"
#include "trick/RefParseContext.hh"

extern int REF_debug;

REF2 *Trick::MemoryManager::ref_attributes(const char* name) {

    std::stringstream reference_sstream;
    REF2 * result = NULL;
    RefParseContext* context = NULL;

    /** @par Design Details: */
    reference_sstream << name;

    REF_debug = 0;

    /** @li Create a parse context. */
    context = new RefParseContext(this, &reference_sstream);

    /** @li Call REF_parse to parse the variable reference. */
    if (context != NULL) {
        int parse_ret = REF_parse(context);
        if ( parse_ret == 0) {
            /* throw away the reference returned by the ref_parser, it isn't the same
               name as requested as "name" */
            if ( context->result->reference ) {
                free(context->result->reference) ;
            }
            context->result->reference = strdup(name);
            result = context->result;
        } else if ( parse_ret == TRICK_PARAMETER_ARRAY_SIZE) {
            /* print out of bounds error message if MM debug_level is greater than 1 */
            if (debug_level > 1) {
                std::stringstream message;
                message << name << " contains out of bounds array index.";
                emitError(message.str());
            }
        } else if ( parse_ret == TRICK_PARAMETER_ADDRESS_NULL) {
            /* print NULL address error message if MM debug_level is greater than 1 */
            if (debug_level > 1) {
                std::stringstream message;
                message << name << " contains NULL address.";
                emitError(message.str());
            }
        }
        /** @li Delete the parse context. */
        delete( context);
    }

    /** @li Return the the REF2 object.*/
    return ( result);
}


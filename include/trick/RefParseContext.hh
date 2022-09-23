
#ifndef REFPARSECONTEXT_HH
#define REFPARSECONTEXT_HH

#include <iostream>
#include <unistd.h>

#include "trick/MemoryManager.hh"
#include "trick/reference.h"

/**
 The class represents the context for a variable-reference parser.
 The context provides stuff that the parser needs to know and
 provides a place to save results.
 */
class RefParseContext {

public:

    int debug ;                      /**< -- Debug level */
    int verify_input;                /**< -- Verify input runstream syntax flag */
    int echo_input;                  /**< -- OBSOLETE? Echo Initialization Data flag */
    REF2 *result;                    /**< ** Resulting REF2 structure from the reference parser.*/
    std::istream *is;                /**< ** Input Stream.*/
    void *scanner;                   /**< ** flex/bision scanner varaible. Magic! */
    Trick::MemoryManager *mem_mgr;   /**< ** Memory Manager. */

    char *save_str_pos;              /**< ** OBSOLETE? saved position in current file */
    char *error_str;                 /**< ** OBSOLETE? Unresolved reference name */

    RefParseContext(Trick::MemoryManager *mem_mgr, std::istream* is );
    ~RefParseContext();

protected:

    /** Initialize the flex scanner for reference parser. Implemented in ref_parser.l. */
    void init_scanner();

    /** Destroy the scanner for reference parser. Implemented in ref_parser.l. */
    void destroy_scanner();

private:

    /** Don't Allow the default constructor to be used. */
    RefParseContext();

};

int REF_parse( RefParseContext* context);

#endif

#ifndef CHKPT_PARSE_CONTEXT_H
#define CHKPT_PARSE_CONTEXT_H
/*
 * $Id: ChkPtParseContext.hh 3637 2014-08-18 16:55:20Z jpenn1 $
 */

#include <iostream>
#include <unistd.h>

#include "sim_services/MemoryManager/include/MemoryManager.hh"
#include "sim_services/MemoryManager/include/reference.h"

/**
 * This class represents the Check Pointer Parser Context.
 */
class ChkPtParseContext {

public: 

    int debug ;                      /**< -- Debug level */
    int verify_input;                /**< -- Verify input runstream syntax flag */
    int echo_input;                  /**< -- Echo Initialization Data flag */
    std::istream *is;                /**< ** Input Stream */
    char *buf;                       /**< ** Temporary storage for strings */
    void *scanner;                   /**< ** Flex scanner variable.*/
    Trick::MemoryManager *mem_mgr;   /**< ** Associated MemoryManager. */

    int bad_declaration_count;       /**< ** Number of bad declarations. */
    int bad_assignment_count;        /**< ** Number of bad assignments. */

    char *save_str_pos;              /**< ** saved position in current file */
    char *error_str;                 /**< ** Unresolved reference name */

    ChkPtParseContext(Trick::MemoryManager *mem_mgr, std::istream* is );
    ~ChkPtParseContext();

protected:

    /**
     Initialize the Flex Scanner.
     */
    void init_scanner();

    /**
     Destroy the Flex Scanner.
     */
    void destroy_scanner();

};

int CCP_parse( ChkPtParseContext* context);

#endif

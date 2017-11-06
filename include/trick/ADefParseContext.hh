#ifndef ADEFPARSECONTEXT_HH
#define ADEFPARSECONTEXT_HH

#include <iostream>
#include <unistd.h>

#include "trick/MemoryManager.hh"

/**
 The class represents the context for an allocation-declaration parser.
 The context provides stuff that the parser needs to know and provides
 a place to save results.
 */
namespace Trick {

class ADefParseContext {

    public:

        int debug ;                          /**< -- Debug level */
        std::istream *is;                    /**< ** Input Stream */
        void         *scanner;               /**< ** Flex scanner. */
        const char   *error_str;             /**< ** Error description. */
        TRICK_TYPE   type;                   /**< ** TRICK_TYPE */
        std::string  user_type_name;         /**< ** User defined type name. */
        int          n_stars;                /**< ** Number of pointers (asterisks) in the definition. */
        std::string  var_name;               /**< ** Name of the variable being defined. */
        int          n_cdims;                /**< ** Number of constrained dimensions. */
        int          cdims[TRICK_MAX_INDEX]; /**< ** Extents of each of the dimensions. */

        ADefParseContext(std::istream* is = &std::cin );
        ~ADefParseContext();

    protected:

        /** Initialize the flex scanner for declaration parser. Implemented in decl_parser.l. */
        void init_scanner();

        /** Destroy the scanner for declaration parser. Implemented in decl_parser.l. */
        void destroy_scanner();

    };
}

int ADEF_parse( Trick::ADefParseContext* context);

#endif

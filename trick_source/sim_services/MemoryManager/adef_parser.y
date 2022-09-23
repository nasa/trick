%name-prefix="ADEF_"
%pure-parser
%locations
%parse-param {Trick::ADefParseContext* context}
%lex-param { void* scanner }
%{

/*
   %debug
   %error-verbose
*/

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "trick/vval.h"
#include "trick/value.h"
#include "trick/var.h"
#include "trick/ADefParseContext.hh"
#include "adef_parser.tab.hpp"

#pragma GCC diagnostic ignored "-Wunused-parameter"

    using namespace std;

    int ADEF_lex( YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner );
 
    void ADEF_error( YYLTYPE* locp, Trick::ADefParseContext* context, const char* err) {
       std::stringstream message;
       message << "adef_parser PARSE-ERROR " << locp->first_line << ": " << err << ".";
       Trick::MemoryManager::emitError(message.str());
    }

#define scanner context->scanner
#ifdef YYDEBUG
#undef YYDEBUG
#endif
#define YYDEBUG 1
%}

%start alloc_definition

%union {
    char*       sval  ;
    int         ival  ;
    VALUE_LIST* value_list ;
}

%token <sval> NAME
%token <ival> TYPE
%token <ival> I_CON

%token END 0 "End of file"

%type  <value_list> dim_list
%type  <ival> pointers
%type  <sval> opt_name user_defined_type

%%

alloc_definition: TYPE pointers opt_name dim_list {

                 context->type = (TRICK_TYPE)$1; 
                 context->user_type_name = "";
                 context->n_stars = $2;
                 if ($3 == NULL) {
                     context->var_name = "";
                 } else {
                     context->var_name = $3;
                     free($3) ;
                 }
             }
           | user_defined_type pointers opt_name dim_list {
 
                 // TRICK_OPAQUE_TYPE means that the type is a user-defined type,
                 // the details of which will be determined later.
                 context->type = TRICK_OPAQUE_TYPE;
                 context->user_type_name = $1;
                 free($1) ;
                 context->n_stars = $2;
                 if ($3 == NULL) {
                     context->var_name = "";
                 } else {
                     context->var_name = $3;
                     free($3) ;
                 }
             }
           ;

opt_name: { $$ = NULL; }
             | NAME { $$ = $1; }
             ;

pointers: {
        /*
         * pointers counts how many pointer dims there are in a variable declaration
         * the first rule handles no pointers, the next handles 1 or more pointers
         */
        $$ = 0 ;
        }
      | pointers '*' {
        $$++ ;
        }
      ;

user_defined_type: NAME {
                       $$ = $1; 
                   }
                 | user_defined_type ':' ':' NAME {
                       size_t len;
                       len = strlen($1) + strlen($4) + (size_t)3; 
                       $$ = (char*)malloc( (size_t)len);
                       strcpy($$, $1);
                       strcat($$, "::");
                       strcat($$, $4);
                       free($1);
                       free($4);
                   }
                 ;

dim_list: {
             /*
              * These two rules handle C style array indexing. This rule
              * handles no indexes, the second rule handles one or more other
              * indexes. 
              */
              int ii ;
              context->n_cdims = 0;
              for ( ii = 0 ; ii < TRICK_MAX_INDEX ; ii++ ) {
                  context->cdims[ii] = 0;
              } 
          }
          | dim_list '[' I_CON ']' {
               if (context->n_cdims >= TRICK_MAX_INDEX ) {
                   context->error_str = "Too many array dimensions." ;
                   return(MM_TOO_MANY_ARRAY_DIMENSIONS) ;
               }
               context->cdims[ context->n_cdims ] = $3 ;
               context->n_cdims++;
         };

%%



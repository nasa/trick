%name-prefix="REF_"
%pure-parser
%locations
%parse-param {RefParseContext* context}
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

#include "trick/RefParseContext.hh"
#include "trick/vval.h"
#include "trick/value.h"
#include "trick/var.h"
#include "ref_parser.tab.h"

    using namespace std;

    int REF_lex( YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner );
 
    void REF_error( YYLTYPE* locp, RefParseContext* context __attribute__ ((unused)) , const char* err) {
        std::stringstream message;
        message << "Syntax error: " << locp->first_line << " : " << err ;
        Trick::MemoryManager::emitError(message.str());
    }

#define scanner context->scanner
#ifdef YYDEBUG
#undef YYDEBUG
#endif
#define YYDEBUG 1
%}

%start reference

%union {
    char*  sval  ;
    int    ival  ;
    V_DATA vval ;
    REF2   rval ;
}

%token <sval> NAME 
%token <ival> I_CON

%token ARROW

%token END 0 "End of file"

%type  <vval> v_data 
%type  <rval> param reference 
%type  <ival> dereference 

%%

reference: param { 
        /*
         * This rule handles the collection of parameter name information.
         * When this rule fires the I->reference parameter, of type REF,
         * (same type used through io_* functions), is ready to be sent to
         * ref_attributes()
         */

          $$ = $1 ;
          context->result = (REF2*)malloc( sizeof(REF2));
          memcpy( context->result, &$1, sizeof(REF2));
}
;

param: NAME {
    /* 
     * This rule handles anything that starts with a NAME.  That includes
     * parameters, input file variables, defines, enumerations, and
     * sizeof structure types.
     * This first rule will return a reference if the variable is a
     * parameter or a variable.
     */

    int ret;

    $$.reference = NULL;
    $$.num_index = 0;
    $$.units = NULL;
    $$.pointer_present = 0;
    $$.ref_type = REF_ADDRESS;
    $$.create_add_path = 1 ;
    $$.address_path = DLL_Create() ;

    // Get the address and attrs of the variable.
    if ((ret = context->mem_mgr->ref_var( &$$, $1)) != MM_OK) {
        return ( ret);
    }

    $$.num_index_left = $$.attr->num_index;
    $$.reference = $1 ;

}
| '&' NAME {
    /*
     * This rule handles the first name with a preceding address character.
     * Only parameters and vars are allowed to have a preceding "&" char.
     */

      int ret;

      $$.reference = NULL;
      $$.num_index = 0;
      $$.units = NULL;
      $$.pointer_present = 0;
      $$.ref_type = REF_ADDRESS;
      $$.create_add_path = 1 ;
      $$.address_path = DLL_Create() ;

    // Get the address and attrs of the variable.
    if ((ret = context->mem_mgr->ref_var( &$$, $2)) != MM_OK) {
        std::stringstream message;
        message << "MemoryManager ERROR: Invalid reference: \"" << $2 << "\".";
        Trick::MemoryManager::emitError(message.str());
        return ( ret);
    }

    $$.num_index_left = $$.attr->num_index;
    //$$.reference = strdup($2) ;

}
| param '[' v_data ']' {

    int ret;

    if ((ret = context->mem_mgr->ref_dim(&$$, &$3)) != MM_OK) {
        return (ret);
    }

}
| param dereference NAME {

    /* This rule handles every name level after the first with a "->" or "." */
    int ret;
    char temp[512];

    if ( $2 == 1) {
        // we have an ARROW, so dereference the address.
        $$.address = *(void**)$$.address;
        $$.num_index ++; 
    }

    /* Check to see if previous parameter specified enough dimensions. */
    if ($$.num_index != $$.attr->num_index) {
        std::stringstream message;
        message << "Dimension mismatch.";
        Trick::MemoryManager::emitError(message.str());
        return (MM_PARAMETER_ARRAY_DIM);
    }

    $$.num_index = 0;

    if ((ret = context->mem_mgr->ref_name(&$$, $3)) != MM_OK) {
        return (ret);
    }

    /* create a new reference string because previous nodes may refer to old strings */
    $$.num_index_left = $$.attr->num_index;

    sprintf(temp , "%s.%s" , $$.reference, $3) ;

    $$.reference = (char*)realloc($$.reference, strlen(temp) + 1) ;
    strcpy($$.reference , temp) ;

    free($3);

} ;

dereference: ARROW {
        $$ = 1 ;
           }
           | '.' {
        $$ = 0 ;
           };

v_data:
    I_CON { 
        /*
         * Do not convert to double to ensure that there is no loss of precision
         */
        $$.value.i = $1 ;
        $$.type = TRICK_INTEGER ;
    }
    ;

%%



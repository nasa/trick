%name-prefix="CCP_"
%pure-parser
%locations
%parse-param {ChkPtParseContext* IP}
%lex-param { void* scanner }
%debug

%{

/*
   %error-verbose
 */

#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string.h>

#include "trick/ChkPtParseContext.hh"
#include "trick/vval.h"
#include "trick/value.h"
#include "trick/var.h"
#include "trick/message_proto.h"
#include "trick/message_type.h"
#include "input_parser.tab.hpp"

    using namespace std;

    int CCP_lex( YYSTYPE* lvalp, YYLTYPE* llocp, void* scanner );

    void CCP_error( YYLTYPE* locp, ChkPtParseContext* context, const char* err) {
       message_publish(MSG_ERROR, "Checkpoint Agent input_parser PARSE-ERROR %d : %s\n", locp->first_line, err) ;
    }

#define scanner IP->scanner

%}

%start entries

%union {
        char                cval  ;
        char*               sval  ;
        long                ival  ;
        long long           llval ;
        unsigned long long  ullval ;
        double              fval  ;
        V_DATA              vval ;
        REF2                rval ;
        VALUE_LIST*         value_list ;
        V_TREE*             v_tree ;
        VAR_LIST            var_list;
}

%token <cval> C_CON
%token <sval> S_CON NAME NAM2 TYPE_NAME 
%token <ival> TYPE 
%token <llval> I_CON
%token <ullval> UI_CON
%token <fval> F_CON

%token ECHO_ON
%token ECHO_OFF
%token GET_REF
%token ALLOC 
%token ARROW
%token DOUBLE_DASH

%token END 0 "End of file"

%type  <vval> v_data initial_value 
%type  <sval> units units_string
%type  <ival> p_list dereference
%type  <rval> param reference 
%type  <value_list> dim_list 
%type  <var_list> name_list 
%type  <v_tree> assignment_item assignment_list 

%%

entries: { 
             if ( IP->mem_mgr->debug_level > 2) {
                  yydebug = 1 ;
             } else {
                  yydebug = 0 ;
             }
         }
         | entries allocate {
         }
         | entries trick_funcs {
         }
         | entries assignment {
         }
         | entries declaration {
         }
         | entries error {

         /* This rule prevents a premature "file" rule reduction ( before
          * yyerror can be called to report the syntax error), when an
          * unexpected token is encountered immediately following an
          * "entries" rule reduction.
          *
          * Note: the special "error" token is described in Section 6 of the
          * GNU Bison Manual.
          */

         } ;

declaration: TYPE name_list ';' { 

                 /*
                  * Declaration of intrinsic data types.
                  */

                 int i , ret = -1 ;

                 if (IP->mem_mgr->add_vars( (TRICK_TYPE)$1 , NULL , &$2 ,NULL ) != 0) {
                     std::stringstream ss;
                     ss << "Checkpoint Agent ERROR: Declaration failed." << std::endl;
                     message_publish( MSG_ERROR, ss.str().c_str() );
                     IP->bad_declaration_count ++;
                 }
                 for ( i = 0 ; i < $2.num_vars ; i++ ) {
                     free($2.var_declare[i].dim_list) ;
                     free($2.var_declare[i].name) ;
                 }
                 free($2.var_declare) ;
             }

           | NAME name_list ';' {

                 /*
                  * Declaration of user-defined (class or struct) data types.
                  */

                 int i , ret = -1 ;

                 if (IP->mem_mgr->add_vars( TRICK_STRUCTURED , $1 , &$2, NULL) != 0) {
                     std::stringstream ss;
                     ss << "Checkpoint Agent ERROR: Declaration failed." << std::endl;
                     message_publish( MSG_ERROR, ss.str().c_str() );
                     IP->bad_declaration_count ++;
                 }
                 free( $1 ) ;
                 for ( i = 0 ; i < $2.num_vars ; i++ ) {
                     free($2.var_declare[i].dim_list) ;
                     free($2.var_declare[i].name) ;
                 }
                 free($2.var_declare) ;
            }
          ;

name_list: p_list NAME dim_list initial_value {

               /*
                * name_list consists of 1 or more variable names to be declared
                * the first rule handles the first variable.  The next handles all
                * subsequent variable names.
                */

               $$.num_vars = 0 ;

               $$.var_declare = (VAR_DECLARE *)malloc(sizeof(VAR_DECLARE)) ;
               $$.var_declare[$$.num_vars].num_pointers = $1 ;
               $$.var_declare[$$.num_vars].name = $2 ;
               $$.var_declare[$$.num_vars].dim_list = $3 ;
               $$.var_declare[$$.num_vars].value = $4 ;

               $$.num_vars++ ;
           }
         | name_list ',' p_list NAME dim_list initial_value {

               $$.var_declare = (VAR_DECLARE *)realloc($$.var_declare , ($$.num_vars + 1) * sizeof(VAR_DECLARE)) ;
               $$.var_declare[$$.num_vars].num_pointers = $3 ;
               $$.var_declare[$$.num_vars].name = $4 ;
               $$.var_declare[$$.num_vars].dim_list = $5 ;
               $$.var_declare[$$.num_vars].value = $6 ;

               $$.num_vars++ ;

           }
         ;

p_list: {
        /*
         * p_list counts how many pointer dims there are in a variable declaration
         * the first rule handles no pointers, the next handles 1 or more pointers
         */
        $$ = 0 ;
        } 
      | p_list '*' {
        $$++ ;
        }
      ;

initial_value: {
                   /*
                    * initial value handles the initial value of a variable in a declaration
                    * the first rule handles no initial value (default to 0) the second rule
                    * passes on value calculated in v_data
                    */

                   $$.type = TRICK_LONG_LONG ;
                   $$.value.ll = 0 ;

               } 
             | '=' v_data {

                   $$ = $2 ;
             }
             ;

trick_funcs: ECHO_ON ';' { 
                 /*
                  * This statement turns ON input file echo. NOTE that I->echo_input
                  * is the SAME parameterused in the input file to turn echo on and
                  * off.
                  */
             }
           | ECHO_OFF ';' { 
                  /*
                   * This statement turns OFF input file echo. NOTE that I->echo_input
                   * is the SAME parameter used in the input file to turn echo on and
                   * off.
                   */
             }
           | NAME '(' v_data ')' ';' { 

                 if (strcmp("debug",$1) == 0) {
                     int debug_level = vval_int(&$3);
                     IP->mem_mgr->debug_level = debug_level;
                     std::stringstream ss;
                     ss << "Checkpoint Agent INFO: DEBUG_LEVEL set to " << debug_level << "." << std::endl;
                     message_publish( MSG_INFO, ss.str().c_str() );
                 }
                 free( $1 ) ;
             }
           | NAME '(' ')' ';' {
                 if (strcmp("clear_all_vars",$1) == 0) {
                     IP->mem_mgr->clear_all_vars();
                 }
                 free( $1 ) ;
             }
           ;

allocate: reference ALLOC '(' v_data ')' ';' { 
              /*
               * This statement handles dynamic memory allocation for
               * unconstrained arrays. The param rule is responsible for
               * collecting all the information about the parameter name and
               * array dimensions, this rule is responsible for calling the
               * ref_allocate() function, in S_source.c, to allocate the
               * specified memory for the specified parameter.
               */

               IP->mem_mgr->ref_allocate( &$1 , vval_int(&$4)) ;

          }
;

assignment: reference assignment_item ';' {
    int ret ;

    if ($1.ref_type != REF_INVALID) {
        if ( $1.attr->io & TRICK_CHKPNT_INPUT ) {
            ret = IP->mem_mgr->ref_assignment( &$1 , $2 ) ;

            if ( ret != MM_OK ) {
                std::stringstream ss;
                ss << "Checkpoint Agent ERROR: Assignment to \"";
                ss <<  $1.reference;
                ss << "\" failed because ref_assignment() failed.";
                ss << std::endl;
                message_publish(MSG_ERROR, ss.str().c_str() );
                IP->bad_assignment_count ++;
            }
        } else {
            std::stringstream ss;
            ss << "Checkpoint Agent WARNING: \"";
            ss << $1.reference;
            ss << "\" not restored because IO specification does not allow checkpoint input.";
            ss << std::endl;
            message_publish(MSG_WARNING, ss.str().c_str() );
            IP->bad_assignment_count ++;
        }
    } else {
        std::stringstream ss;
        ss << "Checkpoint Agent ERROR: Assignment to \"";
        ss <<  $1.reference;
        ss << "\" failed because it's an invalid reference.";
        ss << std::endl;
        message_publish(MSG_ERROR, ss.str().c_str() );
        IP->bad_assignment_count ++;
    }
    delete_v_tree($2);

    /* If the attributes are dynamically-allocated, reference
     * attributes then free them so we don't leak memory.
     */
    if ($1.attr == $1.ref_attr) {
        free($1.ref_attr);
        $1.ref_attr = NULL;
    }
}

assignment_item: v_data {
                     $$ = (V_TREE*)calloc((size_t)1, sizeof(V_TREE));
                     $$->v_data = (V_DATA*)calloc((size_t)1, sizeof(V_DATA));
                     memcpy($$->v_data, &$1, sizeof(V_DATA));
                 }
               | '{' assignment_list '}' {
                     $$ = (V_TREE*)calloc((size_t)1, sizeof(V_TREE));
                     $$->down = $2;
                 }
               ;


assignment_list: {
                     $$ = NULL;
                 } 
               | assignment_item {
                     $$ = $1;
                     $$->last = $$;
                 }
               | assignment_list ',' assignment_item {
                     $$ = $1;
                     $$->last->next = $3;
                     $$->last = $3;
                 }
               ;

reference: param units '=' { 
               $$ = $1 ;
               $$.units = $2 ;
           }
;

units: { // No Units.
           $$ = NULL; 
       }
     | '{' units_string '}' {
           $$ = $2; 
       }
     ;


units_string: NAME {
                  $$ = $1;
              }
            | NAME '.' NAME '.' {
                  $$ = (char*)malloc( strlen($1) + strlen($3) + 3 );
                  sprintf($$, "%s.%s.", $1,$3); 
                  free($1);
                  free($3);
              }
            | DOUBLE_DASH {
                  $$ = NULL;
              }
            | I_CON {
                  if ($1 == 1) {
                      $$ = (char*)malloc(2);
                      sprintf($$, "1");
                  } else {
                      return (MM_INVALID_UNITS);
                  }
              }
            | units_string '*' NAME {
                  int len = strlen($1) + strlen($3);
                  $$ = (char*)malloc((size_t)len+2) ;
                  sprintf($$, "%s*%s",$1,$3);
                  free($1);
                  free($3);
              }
            | units_string '/' NAME {
                  int len = strlen($1) + strlen($3);
                  $$ = (char*)malloc((size_t)len+2) ;
                  sprintf($$, "%s/%s",$1,$3);
                  free($1);
                  free($3);
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
           $$.num_index_left = 0;
           $$.pointer_present = 0;
           $$.ref_type = REF_INVALID;
           $$.address = NULL;
           $$.attr = NULL;
           $$.create_add_path = 0 ;
           $$.address_path = NULL ;

           // Get the address and attrs of the variable.
           if ((ret = IP->mem_mgr->ref_var( &$$, $1)) == MM_OK) {
               $$.ref_type = REF_ADDRESS;
           } else {
               // Is it an enumeration value?
               if ((ret = IP->mem_mgr->get_enumerated($1, &$$.v_data)) == MM_OK) {
                   $$.ref_type = REF_VALUE;
               } else {
                   message_publish(MSG_ERROR, "Checkpoint Agent ERROR: Invalid reference: \"%s\"\n", $1);
                   $$.ref_type = REF_INVALID;
               }
           }
           if ($$.attr != NULL) {
               $$.num_index_left = $$.attr->num_index;
           }
           $$.reference = strdup($1) ;

       }
     | param '[' v_data ']' {

           int ret;
           char temp[512];

           // Update the reference to account for the index.
           // Note that $$ and $1 are the same thing.
           if ($1.ref_type != REF_INVALID) {
               if ((ret = IP->mem_mgr->ref_dim(&$$, &$3)) != MM_OK) {
                   message_publish(MSG_ERROR, "Checkpoint Agent ERROR: Invalid Reference: \"%s[%d]\".\n", $$.reference, vval_int(&$3));
                   $$.ref_type = REF_INVALID;
               }
           }
           sprintf(temp , "%s[%d]" , $$.reference, vval_int(&$3)) ;
           $$.reference = (char*)realloc( $$.reference , strlen(temp) + 1);
           strcpy($$.reference , temp) ;

       }

     | param dereference NAME {

           /* This rule handles every name level after the first with a "->" or "." */
           int ret;
           char temp[512];

           if ($1.ref_type != REF_INVALID) {

               // If the dereference is an arrow ...
               if ($2 == 1) {
                   $$.num_index++;
                   $$.num_index_left--;
               }

               /* Check to see if previous parameter specified too many dimensions. */
               if ($$.num_index > $$.attr->num_index) {
                   message_publish(MSG_ERROR, "Checkpoint Agent ERROR. Invalid Reference: \"%s->%s\".\n", $$.reference, $3);
                   $$.ref_type = REF_INVALID;
               }

               $$.num_index = 0;

               if ((ret = IP->mem_mgr->ref_name(&$$, $3)) != MM_OK) {
                   message_publish(MSG_ERROR, "Checkpoint Agent ERROR: Invalid Reference: \"%s->%s\".\n", $$.reference, $3);
                   $$.ref_type = REF_INVALID;
               }

               /* create a new reference string because previous nodes may refer to old strings */
               $$.num_index_left = $$.attr->num_index;

               if ($2 == 1) {
                   sprintf(temp , "%s->%s" , $$.reference, $3) ;
               } else {
                   sprintf(temp , "%s.%s" , $$.reference, $3) ;
               }

           } else {
               $$.ref_type = REF_INVALID;
           }

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

dim_list: {
             /*
              * These two rules handle C style array indexing. This rule
              * handles no indexes, the second rule handles one or more other
              * indexes. 
              */
              int i ;

              $$ = (VALUE_LIST *)malloc(sizeof(VALUE_LIST)) ;

              $$->num_val = 0 ;
              for ( i = 0 ; i < TRICK_MAX_INDEX ; i++ ) {
                  $$->v_data[i].type = TRICK_INTEGER ;
                  $$->v_data[i].value.i = 0 ;
              }

          }
          | dim_list '[' v_data ']' { 
 
              if ($$->num_val >= TRICK_MAX_INDEX ) {
                  message_publish(MSG_ERROR, "Checkpoint Agent input_parser ERROR 9\n");
                  return(MM_TOO_MANY_ARRAY_DIMENSIONS) ;
              }
              $$->v_data[$$->num_val].type = TRICK_INTEGER ;
              $$->v_data[$$->num_val].value.i = vval_int(&$3) ;
              $$->num_val++ ;
 
         };

v_data: param {
            
              if ($1.ref_type == REF_ADDRESS){  
                  $$.value.vp = (void*)$1.address ;
                  $$.type = TRICK_VOID_PTR ;
              } else if ( $1.ref_type == REF_VALUE ) {
                  $$.value.i = $1.v_data.value.i ;
                  $$.type = $1.v_data.type ;
              } else {
                  message_publish(MSG_ERROR, "Checkpoint Agent ERROR: Reference type is hosed.\n");
                  message_publish(MSG_ERROR, "Please report this error to the Trick Developement Team.\n");
              }
          }

      | '&' param {

            $$.value.vp = (void*)$2.address ;
            $$.type = TRICK_VOID_PTR ;
          }

      | '&' param '+' v_data {
            if (($4.type == TRICK_LONG_LONG)) {
                $$.value.vp = (char*)$2.address + $4.value.ll ;
                $$.type = TRICK_VOID_PTR ;
            } else {
                message_publish(MSG_ERROR, "Checkpoint Agent ERROR: \"+\" is only supported for adding offsets to addresses.\n");
                return ( MM_ERROR);
            }
        }

      | C_CON { 

            $$.value.ll = (long long)$1 ;
            $$.type = TRICK_LONG_LONG ;
          }

      | I_CON { 

            $$.value.ll = $1 ;
            $$.type = TRICK_LONG_LONG ;
          }

      | UI_CON { 

            $$.value.ll = $1 ;
            $$.type = TRICK_LONG_LONG ;
          }

      | F_CON {  

            $$.value.d = $1 ;
            $$.type = TRICK_DOUBLE ;
          }

      | S_CON { 

            $$.value.cp = $1 ;
            $$.type = TRICK_STRING ;
          }
;

%%


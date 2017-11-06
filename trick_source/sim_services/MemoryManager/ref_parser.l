%option prefix="REF_"
%option reentrant
%option bison-bridge
%option bison-locations
%option yylineno
%option noyywrap

%{

/*
 * This is a debug macro which is used to echo every character parsed by the
 * lex.
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "trick/mm_error.h"
#include "trick/RefParseContext.hh"
#include "ref_parser.tab.h"

#define YY_EXTRA_TYPE RefParseContext*

#define YY_USER_ACTION yylloc->first_line = yylineno;

#define YY_INPUT(buf, result, maxsize)            \
{                                                 \
    char c;                                       \
    (*yyextra->is) >> std::noskipws >> c;              \
    if (yyextra->is->eof()) {                     \
        result = YY_NULL;                         \
    } else {                                      \
        buf[0] = c;                               \
        result = 1;                               \
    }                                             \
}

/*===== END OF INITIAL C SOURCE CODE SECTION =====*/
/*
 * Be careful where you put comments after this, lex doesn't like them
 * just anywhere. For example, the IBM needs each comment line to be its
 * own comment - no multi-line comments.
 */
/*=== LEXICAL SPECIFICATIONS ===*/

%}
W     [ \t]
D     [0-9]
OCT   "0"[0-7]+
HEX   "0"[Xx][0-9a-fA-F]+
NAM   [_a-zA-Z][_a-zA-Z0-9:]*

%%

 yy_flex_debug = 0;
 RefParseContext* context = yyextra ;

"." |
"[" |
"]" { return( (int)yytext[0] ); }

{NAM} {
   /*
    * This rule handles general parameter and label names.
    * save the name in a YACC variable and return the token to YACC.
    */
   yylval->sval = strdup( yytext ) ;
   return( NAME ) ;
}

"->" { return ( ARROW ); }

{HEX} {  
   int i ;
   /*
    * This rule handles integers in hexidecimal format.
    * convert the string to an integer value, save the value in
    * the YACC variable and return the YACC token.
    * The first two characters of yytext are the "0x" characters
    * which signify a hex number.
    */
   sscanf( &(yytext[2]) , "%x" , &i ) ;
   yylval->ival = i ;

   //fprintf(stdout,"\n%s: \"%s\"\n",__FILE__, yytext);
   return( I_CON ) ;
}


{D}+ {
   /*
    * This rule handles integers in decimal format.
    * convert the string to an integer value, save the value in
    * the YACC variable and return the YACC token.
    * Decimals can overflow for unsigned long longs so test to
    * see if we are over that and covert to unsigned if it's
    * greater that that huge number
    */
   yylval->ival = atoi(yytext);
   //fprintf(stdout,"\n%s: \"%s\"\n",__FILE__, yytext);
   return( I_CON );
}

<<EOF>> {
   yy_delete_buffer( YY_CURRENT_BUFFER, yyscanner ) ;
   return( 0 );
}


{W} {}


. {
   /*
    * This rule matches all other characters not matched by a previous
    * rule. All lex synatx error messages are handled by the rule.
    * Starting at the unrecognized character, all remaining characters
    * to the end of the current line or the end of the file are read
    * and stored in a buffer which is then used as part of the syntax
    * error message. I->token is an input processor parameter designed
    * specifically for use with error messages.
    */
   context->error_str = yytext ;
   context->save_str_pos = yytext ;
   
   //fprintf(stdout,"\n%s: \"%s\"\n",__FILE__, yytext);
   return(MM_SYNTAX_ERROR) ;
}


%%

void RefParseContext::init_scanner() {

    // Allocate the scanner structure.
    yylex_init( &scanner);

    // Set the file where output messages are to go.
    yyset_out( stdout, scanner);

    yyset_extra( this, scanner);

}

void RefParseContext::destroy_scanner() {

    yylex_destroy(scanner);

}


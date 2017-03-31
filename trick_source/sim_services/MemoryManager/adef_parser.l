%option prefix="ADEF_"
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
#include "trick/ADefParseContext.hh"
#include "adef_parser.tab.h"

#define YY_EXTRA_TYPE Trick::ADefParseContext*

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

/*=== LEXICAL SPECIFICATIONS ===*/

%}
W     [ \t]
D     [0-9]
OCT   "0"[0-7]+
HEX   "0"[Xx][0-9a-fA-F]+
NAM   [_a-zA-Z][_a-zA-Z0-9:]*

%%

 yy_flex_debug = 0;
 Trick::ADefParseContext* context = yyextra ;

"*" |
"[" |
"]" { return( (int)yytext[0] ); }

"char" {
   yylval->ival = TRICK_CHARACTER ;
   return( TYPE ) ;
}

"std::string" {
   yylval->ival = TRICK_STRING ;
   return( TYPE ) ;
}

"wchar" {
   yylval->ival = TRICK_WCHAR ;
   return( TYPE ) ;
}

"signed char" {
   yylval->ival = TRICK_CHARACTER ;
   return( TYPE ) ;
}

"int8_t" {
   yylval->ival = TRICK_CHARACTER ;
   return( TYPE ) ;
}

"unsigned char" {
   yylval->ival = TRICK_UNSIGNED_CHARACTER ;
   return( TYPE ) ;
}

"uint8_t" {
   yylval->ival = TRICK_UNSIGNED_CHARACTER ;
   return( TYPE ) ;
}

"short" {
   yylval->ival = TRICK_SHORT ;
   return( TYPE ) ;
}

"signed short" {
   yylval->ival = TRICK_SHORT ;
   return( TYPE ) ;
}

"int16_t" {
   yylval->ival = TRICK_SHORT ;
   return( TYPE ) ;
}

"unsigned short" {
   yylval->ival = TRICK_UNSIGNED_SHORT ;
   return( TYPE ) ;
}

"uint16_t" {
   yylval->ival = TRICK_UNSIGNED_SHORT ;
   return( TYPE ) ;
}

"int" {
   yylval->ival = TRICK_INTEGER ;
   return( TYPE ) ;
}

"signed int" {
   yylval->ival = TRICK_INTEGER ;
   return( TYPE ) ;
}

"int32_t" {
   yylval->ival = TRICK_INTEGER ;
   return( TYPE ) ;
}

"unsigned int" {
   yylval->ival = TRICK_UNSIGNED_INTEGER ;
   return( TYPE ) ;
}

"uint32_t" {
   yylval->ival = TRICK_UNSIGNED_INTEGER ;
   return( TYPE ) ;
}

"int64_t" {
   yylval->ival = TRICK_INT64 ;
   return( TYPE ) ;
}

"uint64_t" {
   yylval->ival = TRICK_UINT64 ;
   return( TYPE ) ;
}

"long" {
   yylval->ival = TRICK_LONG ;
   return( TYPE ) ;
}

"signed long" {
   yylval->ival = TRICK_LONG ;
   return( TYPE ) ;
}

"unsigned long" {
   yylval->ival = TRICK_UNSIGNED_LONG ;
   return( TYPE ) ;
}

"long long" {
   yylval->ival = TRICK_LONG_LONG ;
   return( TYPE ) ;
}

"signed long long" {
   yylval->ival = TRICK_LONG_LONG ;
   return( TYPE ) ;
}

"unsigned long long" {
   yylval->ival = TRICK_UNSIGNED_LONG_LONG ;
   return( TYPE ) ;
}

"float" {
   yylval->ival = TRICK_FLOAT ;
   return( TYPE ) ;
}

"double" {
   yylval->ival = TRICK_DOUBLE ;
   return( TYPE ) ;
}

"bool" {
   yylval->ival = TRICK_BOOLEAN ;
   return( TYPE ) ;
}

"wchar_t" {
   yylval->ival = TRICK_WCHAR ;
   return( TYPE ) ;
}

{NAM} {
   /*
    * This rule handles general parameter and label names.
    * save the name in a YACC variable and return the token to YACC.
    */
   yylval->sval = strdup( yytext ) ;
   return( NAME ) ;
}


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
   
   return(MM_SYNTAX_ERROR) ;
}


%%

void Trick::ADefParseContext::init_scanner() {

    // Allocate the scanner structure.
    yylex_init( &scanner);

    // Set the file where output messages are to go.
    yyset_out( stdout, scanner);

    yyset_extra( this, scanner);

}

void Trick::ADefParseContext::destroy_scanner() {

    yylex_destroy(scanner);

}


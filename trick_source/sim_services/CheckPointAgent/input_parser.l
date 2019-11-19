%option prefix="CCP_"
%option reentrant
%option bison-bridge
%option bison-locations
%option yylineno
%option debug
%option noyywrap

%{
/*
 * This is a debug macro which is used to echo every character parsed by the
 * lex.
 */
#define PRINT if(I->verify_input || I->echo_input){printf("%s",yytext);}
#define PRINTRED PRINT
#define PRINTGREEN PRINT
#define PRINTYELLOW PRINT
#define PRINTBBLUE PRINT
#define PRINTBMAGENTA PRINT
#define PRINTCYAN PRINT
#define PRINT00

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "trick/mm_error.h"
#include "trick/ChkPtParseContext.hh"
#include "input_parser.tab.hpp"
#include "trick/TrickConstant.hh"

#define YY_EXTRA_TYPE ChkPtParseContext*

#define YY_USER_ACTION yylloc->first_line = yylineno;

#define YY_INPUT(buf, result, maxsize) \
{                                      \
    char c;                            \
    (*yyextra->is) >> std::noskipws >> c;   \
    if (yyextra->is->eof()) {          \
        result = YY_NULL;              \
    } else {                           \
        buf[0] = c;                    \
        result = 1;                    \
    }                                  \
}                                                

/*===== END OF INITIAL C SOURCE CODE SECTION =====*/
/*
 * Be careful where you put comments after this, lex doesn't like them
 * just anywhere. For example, the IBM needs each comment line to be its
 * own comment - no multi-line comments.
   FLOAT (((({D}+"."{D}*)|({D}*"."{D}+)){EXP}?)|({D}+{EXP}))

   FLOAT -?(((({D}+"."{D}*)|({D}*"."{D}+)){EXP}?)|({D}+{EXP}))
 */
/*=== LEXICAL SPECIFICATIONS ===*/

%}
%X QSTR
%X CPPCMT
%X CCOMMENT

W     [ \t]
Q     "\\\""
D     [0-9]
OCT   "0"[0-7]+
HEX   "0"[Xx][0-9a-fA-F]+
DHEX   "0"[Gg][0-9a-fA-F]+
EXP   ([Ee][-+]?{D}+)
FLOAT -?(((({D}+"."{D}*)|({D}*"."{D}+)){EXP}?)|({D}+{EXP}))
CHAR_OCTAL  "'\\"[0-7]+"'"  
CHAR_HEX    "'\\x"[a-fA-F0-9]+"'"  
NAM         [_a-zA-Z][_a-zA-Z0-9:]*
ENAM        [_a-zA-Z][_a-zA-Z0-9.]*
NAM2        [_a-zA-Z][-_a-zA-Z0-9.()\[\]/$#{}]*

ECHO_ON     [Ee][Cc][Hh][Oo][ \t_][Oo][Nn]
ECHO_OFF    [Ee][Cc][Hh][Oo][ \t_][Oo][Ff][Ff]

%%

 yy_flex_debug = 0;
 ChkPtParseContext * I = yyextra ;

"//" BEGIN CPPCMT ;
<CPPCMT>. ;
<CPPCMT>\n BEGIN 0 ;

"/*" { BEGIN(CCOMMENT); }
<CCOMMENT>{
   "*/" { BEGIN(0); }
   . {}
}
<QSTR>"\"" { 

   int l ; PRINTBMAGENTA ;
   /*
    * This rule picks up either 1) the closing double quote of a
    * string, or 2) an escaped quote within a string.
    */
   l = strlen( I->buf ) ;
   if( (l > 0) && (I->buf[ l - 1 ] == '\\') ) {
      /* If the previous character is a backslash then this is an
       * escaped quote, copy the character to the string buffer an
       * continue looking for the real closing quote.
       */
      I->buf[l-1] = '"' ;
   }
   else {
      /* else this is the real closing quote. Save off the
       * character string without the closing quote to the YACC
       * variable, switch the parsing state back to the normal
       * state and return a S_CON token to YACC
       */
      yylval->sval = strdup( I->buf ) ;
      BEGIN 0 ;
      return( S_CON ) ;
   }

}

<QSTR>"\\"  {

     char ch ; 
     PRINTRED ;
     (*yyextra->is) >> ch;
     switch ( ch ) {
        case 'b' :
           strcat ( I->buf , "\b" ) ;
           break ;
        case 't' :
           strcat ( I->buf , "\t" ) ;
           break ;
        case 'n' :
           strcat ( I->buf , "\n" ) ;
           break ;
        case 'v' :
           strcat ( I->buf , "\v" ) ;
           break ;
        case 'f' :
           strcat ( I->buf , "\f" ) ;
           break ;
        case 'r' :
           strcat ( I->buf , "\r" ) ;
           break ;
        case '\\' :
           strcat ( I->buf , "\\" ) ;
           break ;
        case '\'' :
           strcat ( I->buf , "\'" ) ;
           break ;
        case '\"' :
           strcat ( I->buf , "\"" ) ;
           break ;
     }
     if(I->verify_input || I->echo_input){printf("%c",ch);}

}

<QSTR>[^"\\]* {
   PRINTBMAGENTA ; 
   strcat( I->buf , (const char *)yytext ) ;
}


{ECHO_ON} {
   PRINT ;
   return( ECHO_ON ) ;
}

{ECHO_OFF} {
   PRINT ;
   return( ECHO_OFF ) ;
}

";" |
"," |
"&" |
"/" |
"*" |
"+" |
"=" |
"." |
"(" |
")" |
"{" |
"}" |
"[" |
"]" {
    PRINT;
    return( (int)yytext[0] );
}

alloc/{W}*\( {
   PRINT ;
   return( ALLOC ) ;
}

"char" {
   PRINTGREEN ;
   yylval->ival = TRICK_CHARACTER ;
   return( TYPE ) ;
}

"std::string" {
   PRINTGREEN ;
   yylval->ival = TRICK_STRING ;
   return( TYPE ) ;
}

"wchar" {
   PRINTGREEN ;
   yylval->ival = TRICK_WCHAR ;
   return( TYPE ) ;
}

"signed char" {
   PRINTGREEN ;
   yylval->ival = TRICK_CHARACTER ;
   return( TYPE ) ;
}

"unsigned char" {
   PRINTGREEN ;
   yylval->ival = TRICK_UNSIGNED_CHARACTER ;
   return( TYPE ) ;
}

"short" {
   PRINTGREEN ;
   yylval->ival = TRICK_SHORT ;
   return( TYPE ) ;
}

"signed short" {
   PRINTGREEN ;
   yylval->ival = TRICK_SHORT ;
   return( TYPE ) ;
}

"unsigned short" {
   PRINTGREEN ;
   yylval->ival = TRICK_UNSIGNED_SHORT ;
   return( TYPE ) ;
}

"int" {
   PRINTGREEN ;
   yylval->ival = TRICK_INTEGER ;
   return( TYPE ) ;
}

"signed int" {
   PRINTGREEN ;
   yylval->ival = TRICK_INTEGER ;
   return( TYPE ) ;
}

"unsigned int" {
   PRINTGREEN ;
   yylval->ival = TRICK_UNSIGNED_INTEGER ;
   return( TYPE ) ;
}

"long" {
   PRINTGREEN ;
   yylval->ival = TRICK_LONG ;
   return( TYPE ) ;
}

"signed long" {
   PRINTGREEN ;
   yylval->ival = TRICK_LONG ;
   return( TYPE ) ;
}

"unsigned long" {
   PRINTGREEN ;
   yylval->ival = TRICK_UNSIGNED_LONG ;
   return( TYPE ) ;
}

"long long" {
   PRINTGREEN ;
   yylval->ival = TRICK_LONG_LONG ;
   return( TYPE ) ;
}

"signed long long" {
   PRINTGREEN ;
   yylval->ival = TRICK_LONG_LONG ;
   return( TYPE ) ;
}

"unsigned long long" {
   PRINTGREEN ;
   yylval->ival = TRICK_UNSIGNED_LONG_LONG ;
   return( TYPE ) ;
}

"float" {
   PRINTGREEN ;
   yylval->ival = TRICK_FLOAT ;
   return( TYPE ) ;
}

"double" {
   PRINTGREEN ;
   yylval->ival = TRICK_DOUBLE ;
   return( TYPE ) ;
}

"FILE"{W}*\* {
   PRINTGREEN ;
   yylval->ival = TRICK_FILE_PTR ;
   return( TYPE ) ;
}

"bool" {
   PRINTGREEN ;
   yylval->ival = TRICK_BOOLEAN ;
   return( TYPE ) ;
}

"false" {  
  PRINTGREEN ;
  /*
   * This rule handles the c++ bool "false" value.
   */
  yylval->llval = 0 ;
  return( I_CON ) ;
}

"true" {  
   PRINTGREEN ;
   /*
    * This rule handles the c++ bool "true" value.
    */
   yylval->llval = 1 ;
   return( I_CON ) ;
}

"NULL" {
   PRINTBMAGENTA ;
   yylval->llval = 0 ;
   return( I_CON ) ;
}

{NAM} {
   /*
    * This rule handles general parameter and label names.
    * save the name in a YACC variable and return the token to YACC.
    */
   yylval->sval = strdup( yytext ) ;
   PRINT;
   return( NAME ) ;
}

{FLOAT} {
   PRINTBMAGENTA ;
   /*
    * This rule handles a floating point number. 
    * Convert the string to a floating point number, save the number in
    * a YACC variable and return the YACC token.
    */
   yylval->fval = atof( yytext );
   return( F_CON );
}

{OCT} { 
   long long i ; 
   PRINTBMAGENTA ;
   /*
    * This rule handles integers in octal format.
    * convert the string to an integer value, save the value in
    * the YACC variable and return the YACC token.
    */
   sscanf( yytext , "%llo" , &i ) ;
   yylval->llval = i ;
   return( I_CON ) ;
}

{HEX} {  
   long long i ;
   PRINTBMAGENTA ;
   /*
    * This rule handles integers in hexidecimal format.
    * convert the string to an integer value, save the value in
    * the YACC variable and return the YACC token.
    * The first two characters of yytext are the "0x" characters
    * which signify a hex number.
    */
   sscanf( &(yytext[2]) , "%llx" , &i ) ;
   yylval->llval = i ;
   return( I_CON ) ;
}

{DHEX} {  
   long long i ;
   PRINTBMAGENTA ;
   /*
    * This rule handles integers in doubles in hexidecimal format.
    * convert the string to an integer value, save the value in
    * the YACC variable and return the YACC token.
    * The first two characters of yytext are the "0d" characters
    * which signify a hex number.
    * save as a long long but return the type as double
    */
   sscanf( &(yytext[2]) , "%llx" , &i ) ;
   yylval->llval = i ;
   return( F_CON ) ;
}

-?{D}+ {
   PRINTBMAGENTA ;
   /*
    * This rule handles integers in decimal format.
    * convert the string to an integer value, save the value in
    * the YACC variable and return the YACC token.
    * Decimals can overflow for unsigned long longs so test to
    * see if we are over that and covert to unsigned if it's
    * greater that that huge number
    */
#if __APPLE__
   yylval->llval = strtoq( yytext, (char **)NULL, 10 );
#else
   yylval->llval = atoll( yytext );
#endif
   if (yylval->llval == TRICK_MAX_LONG_LONG) {
#if __APPLE__
      yylval->ullval = strtouq(yytext, (char **)NULL, 10);
#else
      yylval->ullval = strtoull(yytext, (char **)NULL, 10);
#endif
      return( UI_CON );
   }
   return( I_CON );
}

-- {
   return( DOUBLE_DASH );
}

"'"."'" {
    PRINTBMAGENTA ;
   /*
    * This rule matches any character enclosed in single quotes.
    * this is used for character assignments.
    * save the character in the YACC variable and return the YACC
    * token.
    */
   yylval->cval = yytext[1] ;
   return( C_CON );
}

{CHAR_OCTAL} {
   unsigned i ;
   PRINTBMAGENTA ;
   /*
    * This rule handles character assignments where the character is
    * specified by its octal integer value. Convert the octal string
    * to an integer value and then save the integer value as a
    * chracter in the YACC variable, then return the YACC token.
    */
   sscanf(&(yytext[2]),"%o",&i);
   yylval->cval = (char)i ;
   return( C_CON );
}

{CHAR_HEX} {
   unsigned i ;
   PRINTBMAGENTA ;
   sscanf(&(yytext[3]),"%x",&i);
   yylval->cval = (char)i ;
   return( C_CON );
}

"'\\n'" {
   PRINTBMAGENTA ;
   /*
    * This rule handles newline character assignments.
    */
   yylval->cval = '\n' ;
   return( C_CON );
}

"'\\t'" {
   PRINTBMAGENTA ;
   /*
    * This rule handles tab character assignments.
    */
   yylval->cval = '\t' ;
   return( C_CON );
}

"'\\\\'" {
   PRINTBMAGENTA ;
   /*
    * This rule handles backslash, '\', character assignments.
    */
   yylval->cval = '\\' ;
   return( C_CON );
}

"\"" {
   PRINTBMAGENTA ;
   /*
    * This is the start of a string in double quotes, throw away
    * the quote character (by not saving it anywhere) initialize
    * a I->buffer to hold the string, and switch the parsing state to a
    * the string only parsing state.
    */
   I->buf[0] = '\0' ;
   BEGIN QSTR ;
}

\n {
   PRINT ;
   /*
    * This rule handles all other newlines.
    */
   //I->file.lineno++ ;
}

[ \t]+ { PRINT } ; /* whitespace */

<QSTR><<EOF>> {
   /* This is an unterminated string */
   I->error_str = I->buf ;
   return(MM_UNTERMINATED_STRING) ;
}

<<EOF>> {
   yy_delete_buffer( YY_CURRENT_BUFFER, yyscanner ) ;
   return( 0 );
}

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
   I->error_str = yytext ;
   I->save_str_pos = yytext ;
   return(MM_SYNTAX_ERROR) ;
}


%%


void ChkPtParseContext::init_scanner() {

    // Allocate the scanner structure.
    yylex_init( &scanner);

    // Set the file where output messages are to go.
    yyset_out( stdout, scanner);
    yyset_extra( this, scanner);

}

void ChkPtParseContext::destroy_scanner() {

    yylex_destroy(scanner);

}


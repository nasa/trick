%{
#include <QList>
#include "product_parser.h"
%}

%option yylineno
%option noyywrap
%option nounput

BACKGROUND_COLOR              [Bb][Aa][Cc][Kk][Gg][Rr][Oo][Uu][Nn][Dd]_[Cc][Oo][Ll][Oo][Rr]
BIAS                          [Bb][Ii][Aa][Ss] 
CHANGE_ONLY                   [Cc][Hh][Aa][Nn][Gg][Ee]_[Oo][Nn][Ll][Yy]
COLUMN_WIDTH                  [Cc][Oo][Ll][Uu][Mm][Nn]_[Ww][Ii][Dd][Tt][Hh] 
CURVE                         [Cc][Uu][Rr][Vv][Ee]
DELIMITER                     [Dd][Ee][Ll][Ii][Mm][Ii][Tt][Ee][Rr]
FONT                          [Ff][Oo][Nn][Tt]
FOREGROUND_COLOR              [Ff][Oo][Rr][Ee][Gg][Rr][Oo][Uu][Nn][Dd]_[Cc][Oo][Ll][Oo][Rr]
FORMAT                        [Ff][Oo][Rr][Mm][Aa][Tt] 
GNUPLOT_FUNCTION_STYLE        [Gg][Nn][Uu][Pp][Ll][Oo][Tt]_[Ff][Uu][Nn][Cc][Tt][Ii][Oo][Nn]_[Ss][Tt][Yy][Ll][Ee]
GNUPLOT_LINE_STYLE            [Gg][Nn][Uu][Pp][Ll][Oo][Tt]_[Ll][Ii][Nn][Ee]_[Ss][Tt][Yy][Ll][Ee]
GNUPLOT_GEOM                  [Gg][Nn][Uu][Pp][Ll][Oo][Tt]_[Gg][Ee][Oo][Mm]
GNUPLOT_OBJECT                [Gg][Nn][Uu][Pp][Ll][Oo][Tt]_[Oo][Bb][Jj][Ee][Cc][Tt]
GNUPLOT_PAGE_ORIENTATION      [Gg][Nn][Uu][Pp][Ll][Oo][Tt]_[Pp][Aa][Gg][Ee]_[Oo][Rr][Ii][Ee][Nn][Tt][Aa][Tt][Ii][Oo][Nn]
GNUPLOT_PLOT_RATIO            [Gg][Nn][Uu][Pp][Ll][Oo][Tt]_[Pp][Ll][Oo][Tt]_[Rr][Aa][Tt][Ii][Oo]
GNUPLOT_TEMPLATE              [Gg][Nn][Uu][Pp][Ll][Oo][Tt]_[Tt][Ee][Mm][Pp][Ll][Aa][Tt][Ee] 
GRID                          [Gg][Rr][Ii][Dd]
GRID_COLOR                    [Gg][Rr][Ii][Dd]_[Cc][Oo][Ll][Oo][Rr]
IN                            [Ii][Nn] 
LABEL                         [Ll][Aa][Bb][Ee][Ll]
LINE_COLOR                    [Ll][Ii][Nn][Ee]_[Cc][Oo][Ll][Oo][Rr] 
LINE_STYLE                    [Ll][Ii][Nn][Ee]_[Ss][Tt][Yy][Ll][Ee] 
MIN_RANGE                     [Mm][Ii][Nn]_[Rr][Aa][Nn][Gg][Ee]
MAX_RANGE                     [Mm][Aa][Xx]_[Rr][Aa][Nn][Gg][Ee]
OUT                           [Oo][Uu][Tt] 
PAGE                          [Pp][Aa][Gg][Ee]
PLOT                          [Pp][Ll][Oo][Tt]
PLOTS                         [Pp][Ll][Oo][Tt][Ss]
PLOT_X_SCALE                  [Pp][Ll][Oo][Tt]_[Xx]_[Ss][Cc][Aa][Ll][Ee]
PLOT_Y_SCALE                  [Pp][Ll][Oo][Tt]_[Yy]_[Ss][Cc][Aa][Ll][Ee]
PLOT_RATIO                    [Pp][Ll][Oo][Tt]_[Rr][Aa][Tt][Ii][Oo]
PROGRAM                       [Pp][Rr][Oo][Gg][Rr][Aa][Mm]
SCALE_FACTOR                  [Ss][Cc][Aa][Ll][Ee]_[Ff][Aa][Cc][Tt][Oo][Rr] 
START                         [Ss][Tt][Aa][Rr][Tt]
STOP                          [Ss][Tt][Oo][Pp]
SYMBOL_SIZE                   [Ss][Yy][Mm][Bb][Oo][Ll]_[Ss][Ii][Zz][Ee]
SYMBOL_STYLE                  [Ss][Yy][Mm][Bb][Oo][Ll]_[Ss][Tt][Yy][Ll][Ee] 
TABLE                         [Tt][Aa][Bb][Ll][Ee]
TABLES                        [Tt][Aa][Bb][Ll][Ee][Ss] 
TIME_NAME                     [Tt][Ii][Mm][Ee]_[Nn][Aa][Mm][Ee]
TIME_UNITS                    [Tt][Ii][Mm][Ee]_[Uu][Nn][Ii][Tt][Ss]
UNITS                         [Uu][Nn][Ii][Tt][Ss]
VARIABLE                      [Vv][Aa][Rr][Ii][Aa][Bb][Ll][Ee]
X_AXIS_LABEL                  [Xx]_[Aa][Xx][Ii][Ss]_[Ll][Aa][Bb][Ee][Ll]
X_MIN_RANGE                   [Xx]_[Mm][Ii][Nn]_[Rr][Aa][Nn][Gg][Ee] 
X_MAX_RANGE                   [Xx]_[Mm][Aa][Xx]_[Rr][Aa][Nn][Gg][Ee] 
X_VARIABLE                    [Xx]_[Vv][Aa][Rr][Ii][Aa][Bb][Ll][Ee]
XY_PAIR                       [Xx][Yy]_[Pp][Aa][Ii][Rr]
Y_AXIS_LABEL                  [Yy]_[Aa][Xx][Ii][Ss]_[Ll][Aa][Bb][Ee][Ll]
Y_AXIS_FORMAT                 [Yy]_[Aa][Xx][Ii][Ss]_[Ff][Oo][Rr][Mm][Aa][Tt]
Y_MIN_RANGE                   [Yy]_[Mm][Ii][Nn]_[Rr][Aa][Nn][Gg][Ee] 
Y_MAX_RANGE                   [Yy]_[Mm][Aa][Xx]_[Rr][Aa][Nn][Gg][Ee] 
Y_VARIABLE                    [Yy]_[Vv][Aa][Rr][Ii][Aa][Bb][Ll][Ee]
MAJOR_X_TICS                  [Mm][Aa][Jj][Oo][Rr]_[Xx]_[Tt][Ii][Cc][Ss]
MAJOR_Y_TICS                  [Mm][Aa][Jj][Oo][Rr]_[Yy]_[Tt][Ii][Cc][Ss]
MINOR_X_TICS                  [Mm][Ii][Nn][Oo][Rr]_[Xx]_[Tt][Ii][Cc][Ss]
MINOR_Y_TICS                  [Mm][Ii][Nn][Oo][Rr]_[Yy]_[Tt][Ii][Cc][Ss]
RECT                          [Rr][Ee][Cc][Tt]
PRESENTATION                  [Pp][Rr][Ee][Ss][Ee][Nn][Tt][Aa][Tt][Ii][Oo][Nn]

QSTR                          \"[^"\n]*["\n] 
STR                           [a-zA-Z][-_a-zA-Z0-9.\,()\[\]/$#{}]* 
FLOAT                         -?(([0-9]+)|([0-9]*\.[0-9]+))([eE][-+]?[0-9]+)?
W                             [ \t]+

%%
 
{BACKGROUND_COLOR}         { return(DP_BACKGROUND_COLOR); }
{BIAS}                     { return(DP_BIAS); }
{CHANGE_ONLY}              { return(DP_CHANGE_ONLY); }
{COLUMN_WIDTH}             { return(DP_COLUMN_WIDTH); }
{CURVE}                    { return(DP_CURVE); }
{DELIMITER}                { return(DP_DELIMITER); }
{FONT}                     { return(DP_FONT); }
{FOREGROUND_COLOR}         { return(DP_FOREGROUND_COLOR); }
{FORMAT}                   { return(DP_FORMAT); }
{GNUPLOT_FUNCTION_STYLE}   { return(DP_GNUPLOT_FUNCTION_STYLE); }
{GNUPLOT_LINE_STYLE}       { return(DP_GNUPLOT_LINE_STYLE); }
{GNUPLOT_GEOM}             { return(DP_GNUPLOT_GEOM); }
{GNUPLOT_OBJECT}           { return(DP_GNUPLOT_OBJECT); }
{GNUPLOT_PAGE_ORIENTATION} { return(DP_GNUPLOT_PAGE_ORIENTATION); }
{GNUPLOT_PLOT_RATIO}       { return(DP_GNUPLOT_PLOT_RATIO); }
{GNUPLOT_TEMPLATE}         { return(DP_GNUPLOT_TEMPLATE); }
{GRID_COLOR}               { return(DP_GRID_COLOR); }
{GRID}                     { return(DP_GRID); }
{IN}                       { return(DP_PROGRAM_IN); }
{LABEL}                    { return(DP_LABEL); }
{LINE_COLOR}               { return(DP_LINE_COLOR); }
{LINE_STYLE}               { return(DP_LINE_STYLE); }
{MAX_RANGE}                { return(DP_MAX_RANGE); }
{MIN_RANGE}                { return(DP_MIN_RANGE); }
{OUT}                      { return(DP_PROGRAM_OUT); }
{PAGE}                     { return(DP_PAGE); }
{PLOTS}                    { return(DP_PLOTS); }
{PLOT}                     { return(DP_PLOT); }
{PLOT_X_SCALE}             { return(DP_PLOT_X_SCALE); }
{PLOT_Y_SCALE}             { return(DP_PLOT_Y_SCALE); }
{PLOT_RATIO}               { return(DP_PLOT_RATIO); }
{PROGRAM}                  { return(DP_PROGRAM); }
{SCALE_FACTOR}             { return(DP_SCALE_FACTOR); }
{START}                    { return(DP_START); }
{STOP}                     { return(DP_STOP); }
{SYMBOL_SIZE}              { return(DP_SYMBOL_SIZE); }
{SYMBOL_STYLE}             { return(DP_SYMBOL_STYLE); }
{TABLES}                   { return(DP_TABLES); }
{TABLE}                    { return(DP_TABLE); }
{TIME_NAME}                { return(DP_TIME_NAME); }
{TIME_UNITS}               { return(DP_TIME_UNITS); }
{UNITS}                    { return(DP_UNITS); }
{VARIABLE}                 { return(DP_VARIABLE); }
{X_AXIS_LABEL}             { return(DP_X_AXIS_LABEL); }
{X_MAX_RANGE}              { return(DP_X_MAX_RANGE); }
{X_MIN_RANGE}              { return(DP_X_MIN_RANGE); }
{X_VARIABLE}               { return(DP_X_VARIABLE); }
{XY_PAIR}                  { return(DP_XY_PAIR); }
{Y_AXIS_FORMAT}            { return(DP_Y_AXIS_FORMAT); }
{Y_AXIS_LABEL}             { return(DP_Y_AXIS_LABEL); }
{Y_MAX_RANGE}              { return(DP_Y_MAX_RANGE); }
{Y_MIN_RANGE}              { return(DP_Y_MIN_RANGE); }
{Y_VARIABLE}               { return(DP_Y_VARIABLE); }
{MAJOR_X_TICS}             { return(DP_MAJOR_X_TICS); }
{MAJOR_Y_TICS}             { return(DP_MAJOR_Y_TICS); }
{MINOR_X_TICS}             { return(DP_MINOR_X_TICS); }
{MINOR_Y_TICS}             { return(DP_MINOR_Y_TICS); }
{RECT}                     { return(DP_RECT); }
{PRESENTATION}             { return(DP_PRESENTATION); }

{QSTR}  { 

        unsigned int ii ;

        // Take off double quotes
        for ( ii = 1 ; ii < strlen(yytext) - 1 ; ii++ ) {
                yylval.sval[ii-1]  = yytext[ii] ;
        }
        yylval.sval[ii-1] = '\0' ;

        return(DP_STR) ; 
}

{STR}  { 
        strcpy(yylval.sval, yytext) ;
        return(DP_STR) ; 
}

{FLOAT}   { 
        yylval.dval = atof( yytext ) ;
        return( DP_FLOAT ) ; 
}

{W} { /* Ignore white space */ }

\n  { /* fprintf(stderr, "line: %d\n",  yylineno);*/ ; yylval.ival = yylineno ; }

.   { /*fprintf(stderr, "%c\n", yytext[0]);*/ ;
      yylval.ival = yylineno ;
      return yytext[0] ;
    }
%%

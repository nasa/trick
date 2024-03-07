
/*
 *   Taken from Trick 05.8.0
 */

%{


#include <stdio.h>
#include <stdlib.h>
#include <QString>
#include <QList>

extern int yylex(void);
extern int yylineno;
extern char* yytext;

QList<double> globalFloatList;

QString dpFileName();

void yyerror(const char *str)
{
    Q_UNUSED(str);
    QString msg;
    msg += "koviz [error] : DP product parser error \n";
    msg += "  Error found in file: " + dpFileName() + "\n";
    msg += "  Error found on line: " + QString("%1").arg(yylineno) + "\n";
    msg += "      Last Token Read: " + QString(yytext) + "\n";
    fprintf(stderr, "%s\n",msg.toLatin1().constData());
    exit(-1);
}

int yywrap()
{
  return 1;
}

void msg(const QString& str, bool isPrint=true)
{
    if ( isPrint ) {
        fprintf(stderr,"koviz [warning]: %s\n",str.toLatin1().constData());
    }
}


#include "dp.h"
extern DPProduct* product;

bool isXYPair = false;
bool isXVar = false;
DPTable* currTable = 0;
DPVar* currTableVar = 0;
DPPage* currPage = 0;
DPPlot* currPlot = 0;
DPCurve* currCurve = 0;
DPVar* currXVar = 0;
DPVar* currYVar = 0;
DPVar* currXYPairXVar = 0;
DPVar* currXYPairYVar = 0;

QString dpFileName() {
    QString fname;
    if ( product ) {
        fname = product->dpFileName();
    }
    return fname;
}


%}

%union {
        int ival ;
        double dval ;
        char sval[1024] ;
        QList<double>* listOfFloatsPtr;
};


%token DP_PAGE DP_PLOT DP_START DP_STOP
%token DP_PLOTS DP_FOREGROUND_COLOR DP_BACKGROUND_COLOR
%token DP_CURVE DP_XY_PAIR
%token DP_FONT DP_GRID DP_GRID_COLOR
%token DP_X_AXIS_LABEL DP_Y_AXIS_LABEL DP_Y_AXIS_FORMAT
%token DP_X_MIN_RANGE DP_X_MAX_RANGE DP_Y_MIN_RANGE DP_Y_MAX_RANGE
%token DP_X_VARIABLE DP_Y_VARIABLE DP_VARIABLE DP_LABEL DP_UNITS
%token DP_TIME_NAME DP_TIME_UNITS
%token DP_SCALE_FACTOR DP_BIAS DP_LINE_COLOR DP_LINE_STYLE
%token DP_SYMBOL_SIZE DP_SYMBOL_STYLE DP_SYMBOL_END
%token DP_MIN_RANGE DP_MAX_RANGE DP_TABLES DP_TABLE
%token DP_FORMAT DP_CHANGE_ONLY DP_COLUMN_WIDTH DP_DELIMITER
%token DP_PROGRAM DP_PROGRAM_IN DP_PROGRAM_OUT DP_GNUPLOT_TEMPLATE DP_GNUPLOT_GEOM
%token DP_GNUPLOT_PLOT_RATIO DP_GNUPLOT_FUNCTION_STYLE 
%token DP_GNUPLOT_LINE_STYLE
%token DP_GNUPLOT_PAGE_ORIENTATION DP_GNUPLOT_OBJECT
%token DP_PLOT_X_SCALE DP_PLOT_Y_SCALE
%token DP_PLOT_RATIO
%token DP_MAJOR_X_TICS DP_MAJOR_Y_TICS
%token DP_MINOR_X_TICS DP_MINOR_Y_TICS
%token DP_RECT
%token DP_PRESENTATION

%token <sval> DP_STR
%token <dval> DP_FLOAT

%type <listOfFloatsPtr> float_list

%%

statements: {
            currTable = 0;
            currTableVar = 0;
            currPage = 0;
            currPlot = 0;
            currCurve = 0;
            currXVar = 0;
            currYVar = 0;
            currXYPairXVar = 0;
            currXYPairYVar = 0;
        }
        | statements program
        | statements { isXYPair = false; isXVar = false; } plot_section
        | statements table_section
        ;

program: DP_PROGRAM ':' DP_STR {
                product->program()->setFileName($3);
        }
        | program program_input
        | program program_output
        ;

program_input: DP_PROGRAM_IN ':' DP_STR {
                product->program()->addInput($3);
        }
        | program_input DP_STR {
                product->program()->addInput($2);
        }
        ;

program_output: DP_PROGRAM_OUT ':' DP_STR {
                product->program()->addOutput($3);
        }
        | program_output DP_STR {
                product->program()->addOutput($2);
        }
        ;

plot_section: DP_PLOTS ':' pages
        ;

pages: page
        | pages page
        ;

page: DP_PAGE DP_FLOAT ':' DP_STR
      { currPage = product->addPage($4); }
      page_options plots
        ;

page_options:
        | page_options DP_GNUPLOT_TEMPLATE ':' DP_STR { 
                msg("PAGE.GNUPLOT_TEMPLATE not supported");
        }
        | page_options DP_GNUPLOT_OBJECT ':' DP_STR { 
                msg("PAGE.GNUPLOT_OBJECT not supported");
        }
        | page_options DP_GNUPLOT_GEOM ':' DP_STR {
                msg("PAGE.GNUPLOT_GEOM not supported");
        }
        | page_options DP_GNUPLOT_PLOT_RATIO ':' DP_STR {
                msg("PAGE.GNUPLOT_RATIO not supported");
        }
        | page_options DP_GNUPLOT_PAGE_ORIENTATION ':' DP_STR {
                msg("PAGE.GNUPLOT_PAGE_ORIENTATION not supported");
        }
        | page_options DP_START ':' DP_FLOAT {
                currPage->setStartTime($4);
        }
        | page_options DP_STOP ':' DP_FLOAT {
                currPage->setStopTime($4);
        }
        | page_options DP_FOREGROUND_COLOR ':' DP_STR  {
                currPage->setForegroundColor($4);
        }
        | page_options DP_BACKGROUND_COLOR ':' DP_STR  {
                currPage->setBackgroundColor($4);
        }
        | page_options DP_FONT ':' DP_STR  {
                msg("PAGE.FONT spec not supported");
        }
        ;

plots: plot
        | plots plot
        ;

plot: DP_PLOT DP_FLOAT ':' DP_STR {
                // DP_STR is plot title
                currPlot = currPage->addPlot($4);
        }
        | plot DP_FOREGROUND_COLOR ':' DP_STR  {
                currPlot->setForegroundColor($4);
        }
        | plot DP_BACKGROUND_COLOR ':' DP_STR  {
                currPlot->setBackgroundColor($4);
        }
        | plot DP_FONT ':' DP_STR  {
                currPlot->setFont($4);
        }
        | plot DP_X_AXIS_LABEL ':' DP_STR {
                currPlot->setXAxisLabel($4);
        }
        | plot DP_PLOT_X_SCALE ':' DP_STR {
                currPlot->setPlotXScale($4);
        }
        | plot DP_X_MIN_RANGE ':' DP_FLOAT {
                currPlot->setXMinRange($4);
        }
        | plot DP_X_MAX_RANGE ':' DP_FLOAT {
                currPlot->setXMaxRange($4);
        }
        | plot DP_Y_AXIS_LABEL ':' DP_STR {
                currPlot->setYAxisLabel($4);
        }
        | plot DP_PLOT_Y_SCALE ':' DP_STR {
                currPlot->setPlotYScale($4);
        }
        | plot DP_PLOT_RATIO ':' DP_STR {
                currPlot->setPlotRatio($4);
        }
        | plot DP_Y_AXIS_FORMAT ':' DP_STR {
                msg("PLOT.Y_AXIS_FORMAT not supported");
        }
        | plot DP_Y_MIN_RANGE ':' DP_FLOAT {
                currPlot->setYMinRange($4);
        }
        | plot DP_Y_MAX_RANGE ':' DP_FLOAT {
                currPlot->setYMaxRange($4);
        }
        | plot DP_GRID ':' DP_STR  {
                currPlot->setGrid($4);
        }
        | plot DP_GRID_COLOR ':' DP_STR  {
                currPlot->setGridColor($4);
        }
        | plot DP_START ':' DP_FLOAT {
                currPlot->setStartTime($4);
        }
        | plot DP_STOP ':' DP_FLOAT {
                currPlot->setStopTime($4);
        }
        | plot DP_MAJOR_X_TICS ':' float_list {
                currPlot->setMajorXTics(*$4);
        }
        | plot DP_MAJOR_Y_TICS ':' float_list {
                currPlot->setMajorYTics(*$4);
        }
        | plot DP_MINOR_X_TICS ':' float_list {
                currPlot->setMinorXTics(*$4);
        }
        | plot DP_MINOR_Y_TICS ':' float_list {
                currPlot->setMinorYTics(*$4);
        }
        | plot DP_RECT ':' DP_FLOAT ',' DP_FLOAT ',' DP_FLOAT ',' DP_FLOAT {
                currPlot->setRect($4,$6,$8,$10);
        }
        | plot DP_PRESENTATION ':' DP_STR  {
                currPlot->setPresentation($4);
        }
        | plot {
                   if ( !isXYPair ) {
                       currCurve = currPlot->addCurve();
                       isXVar = true;
                   }
               }

               x_var

        | plot {
                 if ( !isXVar ) {
                     currCurve = currPlot->addCurve();
                     if ( !currXVar ) {
                         currXVar = currCurve->setXVarName("");
                         currXVar->setUnit("");
                         currXVar->setLabel("");
                     } else {
                         QString xunit = currXVar->unit();
                         QString xlabel = currXVar->label();
                         currXVar = currCurve->setXVarName(
                                       currXVar->name().toLatin1().constData());
                         currXVar->setUnit(xunit.toLatin1().constData());
                         currXVar->setLabel(xlabel.toLatin1().constData());
                     }
                 }
               }

               y_var

               {
                   isXYPair = false ;
                   isXVar = false;
               }

        | plot curve 
        ;

x_var: DP_X_VARIABLE ':' DP_STR {
                // DP_STR is x variable name
                if ( isXYPair ) {
                    currXYPairXVar = new DPVar($3);
                } else {
                    currXVar = currCurve->setXVarName($3);
                }
        }
        | x_var DP_LABEL ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairXVar->setLabel($4);
                } else {
                    currXVar->setLabel($4);
                }
        }
        | x_var DP_UNITS ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairXVar->setUnit($4);
                } else {
                    currXVar->setUnit($4);
                }
        }
        | x_var DP_MIN_RANGE ':' DP_FLOAT {
                currXVar->setMinRange($4);
        }
        | x_var DP_MAX_RANGE ':' DP_FLOAT {
                currXVar->setMaxRange($4);
        }
        | x_var DP_SCALE_FACTOR ':' DP_FLOAT {
                if ( isXYPair ) {
                    currXYPairXVar->setScaleFactor($4);
                } else {
                    currXVar->setScaleFactor($4);
                }
        }
        | x_var DP_BIAS ':' DP_FLOAT {
                if ( isXYPair ) {
                    currXYPairXVar->setBias($4);
                } else {
                    currXVar->setBias($4);
                }
        }
        | x_var DP_TIME_NAME ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairXVar->setTimeName($4);
                } else {
                    currXVar->setTimeName($4);
                }
        }
        | x_var DP_TIME_UNITS ':' DP_STR {
                msg("X_VARIABLE.TIME_UNITS not supported");
        }
        ;

y_var: DP_Y_VARIABLE ':' DP_STR {
                // DP_STR is y variable name
                if ( isXYPair ) {
                    currXYPairYVar = new DPVar($3);
                } else {
                    currYVar = currCurve->setYVarName($3);
                }
        }
        | y_var DP_LABEL ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairYVar->setLabel($4);
                } else {
                    currYVar->setLabel($4);
                }
        }
        | y_var DP_UNITS ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairYVar->setUnit($4);
                } else {
                    currYVar->setUnit($4);
                }
        }
        | y_var DP_MIN_RANGE ':' DP_FLOAT  {
                currYVar->setMinRange($4);
        }
        | y_var DP_MAX_RANGE ':' DP_FLOAT {
                currYVar->setMaxRange($4);
        }
        | y_var DP_SCALE_FACTOR ':' DP_FLOAT {
                if ( isXYPair ) {
                    currXYPairYVar->setScaleFactor($4);
                } else {
                    currYVar->setScaleFactor($4);
                }
        }
        | y_var DP_BIAS ':' DP_FLOAT {
                if ( isXYPair ) {
                    currXYPairYVar->setBias($4);
                } else {
                    currYVar->setBias($4);
                }
        }
        | y_var DP_LINE_STYLE ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairYVar->setLineStyle($4);
                } else {
                    currYVar->setLineStyle($4);
                }
        }
        | y_var DP_LINE_COLOR ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairYVar->setLineColor($4);
                } else {
                    currYVar->setLineColor($4);
                }
        }
        | y_var DP_SYMBOL_STYLE ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairYVar->setSymbolStyle($4);
                } else {
                    currYVar->setSymbolStyle($4);
                }
        }
        | y_var DP_SYMBOL_END ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairYVar->setSymbolEnd($4);
                } else {
                    currYVar->setSymbolEnd($4);
                }
        }
        | y_var DP_GNUPLOT_FUNCTION_STYLE ':' DP_STR {
                msg("Y_VARIABLE.GNUPLOT_FUNCTION_STYLE not supported");
        }
        | y_var DP_TIME_NAME ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairYVar->setTimeName($4);
                } else {
                    currYVar->setTimeName($4);
                }
        }
        | y_var DP_TIME_UNITS ':' DP_STR {
                msg("Y_VARIABLE.TIME_UNITS not supported");
        }
        | y_var DP_GNUPLOT_LINE_STYLE ':' DP_STR {
                msg("Y_VARIABLE.GNUPLOT_LINE_STYLE not supported");
        }
        ;

curve: DP_CURVE ':' {
                currCurve = currPlot->addCurve();
        }
        | curve DP_XY_PAIR ':' '{'
                { isXYPair = true;
                  currXYPairXVar = new DPVar("");  // No X Specified, make empty
                } y_var '}' {

                currCurve->addXYPair(currXYPairXVar,currXYPairYVar);
                isXYPair = false;
        }
        | curve DP_XY_PAIR  ':' '{' {isXYPair = true;} x_var y_var '}' {
                currCurve->addXYPair(currXYPairXVar,currXYPairYVar);
                isXYPair = false;
        }
        | curve DP_LABEL ':' DP_STR {
                msg("CURVE.LABEL not supported.  "
                    "Use [XY]_VARIABLE.LABEL instead.");
        }
        | curve DP_LINE_STYLE ':' DP_STR {
                msg("CURVE.LINE_STYLE not supported.  "
                    "Use Y_VARIABLE.LINE_STYLE instead.");
        }
        | curve DP_LINE_COLOR ':' DP_STR {
                currCurve->setLineColor($4);
        }
        | curve DP_SYMBOL_STYLE ':' DP_STR {
                msg("CURVE.LINE_SYMBOL not supported.  "
                    "Use Y_VARIABLE.SYMBOL_STYLE instead.");
        }
        | curve DP_SYMBOL_SIZE ':' DP_STR {
                msg("CURVE.SYMBOL_SIZE not supported.  "
                    "Use Y_VARIABLE.SYMBOL_SIZE instead.");
        }
        | curve DP_GNUPLOT_FUNCTION_STYLE ':' DP_STR {
                msg("CURVE.GNUPLOT_FUNCTION_STYLE not supported");
        }
        | curve DP_GNUPLOT_LINE_STYLE ':' DP_STR {
                msg("CURVE.GNUPLOT_LINE_STYLE not supported");
        }
        ;

float_list: DP_FLOAT {
                QList<double>* list = &globalFloatList;
                list->clear();
                list->append($1);
                $$ = list;
        }
        | float_list ',' DP_FLOAT {
                $1->append($3);
                $$ = $1;
        }
        ;


table_section: DP_TABLES ':' tables
        ;

tables: table
        | tables table
        ;

table: DP_TABLE DP_FLOAT ':' DP_STR
       { currTable = product->addTable($4); }
       table_options table_vars
       ;

table_options:
        | table_options DP_START ':' DP_FLOAT {
                currTable->setStartTime($4);
        }
        | table_options DP_STOP ':' DP_FLOAT {
                currTable->setStopTime($4);
        }
        | table_options DP_DELIMITER ':' DP_STR {
                currTable->setDelimiter($4);
        }
        | table_options DP_CHANGE_ONLY ':' DP_STR {
                msg("TABLE.CHANGE_ONLY not supported");
        }
        | table_options DP_COLUMN_WIDTH ':' DP_FLOAT {
                msg("TABLE.COLUMN_WIDTH not supported");
        }
        ;

table_vars: table_var
        | table_vars table_var
        ;

table_var: DP_VARIABLE ':' DP_STR {
                // DP_STR is variable name
                currTableVar = currTable->addVar($3);
        }
        | table_var DP_LABEL ':' DP_STR {
                currTableVar->setLabel($4);
        }
        | table_var DP_TIME_NAME ':' DP_STR {
                currTableVar->setTimeName($4);
        }
        | table_var DP_UNITS ':' DP_STR {
                currTableVar->setUnit($4);
        }
        | table_var DP_SCALE_FACTOR ':' DP_FLOAT {
                currTableVar->setScaleFactor($4);
        }
        | table_var DP_BIAS ':' DP_FLOAT {
                currTableVar->setBias($4);
        }
        | table_var DP_FORMAT ':' DP_STR {
                currTableVar->setFormat($4);
        }
        | table_var DP_MIN_RANGE ':' DP_FLOAT {
                currTableVar->setMinRange($4);
        }
        | table_var DP_MAX_RANGE ':' DP_FLOAT {
                currTableVar->setMaxRange($4);
        }
        ;
%%

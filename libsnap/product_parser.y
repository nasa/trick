
/*
 *   Taken from Trick 05.8.0
 */

%{


#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <QString>
#include <QDebug>
#include <stdexcept>
using namespace std ;

extern int yylex(void);
extern int yylineno;
extern char* yytext;

QString dpFileName();

void yyerror(const char *str)
{
    Q_UNUSED(str);
    QString msg;
    msg += "snap dp: [error] : DP product parser error \n";
    msg += "  Error found in file: " + dpFileName() + "\n";
    msg += "  Error found on line: " + QString("%1").arg(yylineno) + "\n";
    msg += "      Last Token Read: " + QString(yytext) + "\n";
    throw std::runtime_error(msg.toAscii().constData());
}

int yywrap()
{
  return 1;
}

void msg(const QString& str, bool isPrint=false)
{
    if ( isPrint ) qDebug() << str;
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
}


%token DP_PAGE DP_PLOT DP_START DP_STOP
%token DP_PLOTS DP_FOREGROUND_COLOR DP_BACKGROUND_COLOR
%token DP_CURVE DP_XY_PAIR
%token DP_FONT DP_GRID DP_GRID_COLOR
%token DP_X_AXIS_LABEL DP_Y_AXIS_LABEL DP_Y_AXIS_FORMAT
%token DP_X_SCALE DP_Y_SCALE
%token DP_X_MIN_RANGE DP_X_MAX_RANGE DP_Y_MIN_RANGE DP_Y_MAX_RANGE
%token DP_X_VARIABLE DP_Y_VARIABLE DP_VARIABLE DP_LABEL DP_UNITS
%token DP_TIME_NAME DP_TIME_UNITS
%token DP_SCALE_FACTOR DP_BIAS DP_LINE_COLOR DP_LINE_STYLE DP_SYMBOL_SIZE
%token DP_SYMBOL_STYLE DP_MIN_RANGE DP_MAX_RANGE DP_TABLES DP_TABLE
%token DP_FORMAT DP_CHANGE_ONLY DP_COLUMN_WIDTH DP_DELIMITER
%token DP_PROGRAM DP_PROGRAM_IN DP_PROGRAM_OUT DP_GNUPLOT_TEMPLATE DP_GNUPLOT_GEOM
%token DP_GNUPLOT_PLOT_RATIO DP_GNUPLOT_FUNCTION_STYLE 
%token DP_GNUPLOT_LINE_STYLE
%token DP_GNUPLOT_PAGE_ORIENTATION DP_GNUPLOT_OBJECT

%token <sval> DP_STR
%token <dval> DP_FLOAT

%%

statements:
        | statements program
        | statements { isXYPair = false; isXVar = false; } plot_section
        | statements table_section
        ;

program: DP_PROGRAM ':' DP_STR {
                // DP_STR is program name
                //product->addProgram($3);
                msg("product->addProgram() unsupported");
        }
        | program program_input
        | program program_output
        ;

program_input: DP_PROGRAM_IN ':' DP_STR {
                //product->addProgramInput($3);
                msg("product->addProgramInput() unsupported");
        }
        | program_input DP_STR {
                //product->addProgramInput($2);
                msg("product->addProgramInput() unsupported");
        }
        ;

program_output: DP_PROGRAM_OUT ':' DP_STR {
                //product->addProgramOutput($3);
                msg("product->addProgramOutput() unsupported");
        }
        | program_output DP_STR {
                //product->addProgramOutput($2);
                msg("product->addProgramOutput() unsupported");
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
                // DP_STR == template location on file system
                //product->getCurrPage()->setGnuplotTemplate($4);
                msg("page->setGnuplotTemplate() not supported");
        }
        | page_options DP_GNUPLOT_OBJECT ':' DP_STR { 
                // DP_STR == gnuplot object 
                //product->getCurrPage()->setGnuplotObject($4);
                msg("page->setGnuplotObject() not supported");
        }
        | page_options DP_GNUPLOT_GEOM ':' DP_STR {
                //product->getCurrPage()->setGnuplotGeom($4);
                msg("page->setGnuplotGeom() not supported");
        }
        | page_options DP_GNUPLOT_PLOT_RATIO ':' DP_STR {
                //product->getCurrPage()->setGnuplotPlotRatio($4);
                msg("page->setGnuplotPlotRatio() not supported");
        }
        | page_options DP_GNUPLOT_PAGE_ORIENTATION ':' DP_STR {
                //product->getCurrPage()->setGnuplotPageOrientation($4);
                msg("page->setGnuplotPageOrientation() not supported");
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
                //product->getCurrPage()->setFont($4);
                msg("page->setGnuplotPageOrientation() not supported");
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
        | plot DP_X_SCALE ':' DP_STR {
                //currPlot->setXLogScale($4);
                msg("page->setXLogScale() not supported");
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
        | plot DP_Y_SCALE ':' DP_STR {
                //currPlot->setYLogScale($4);
                msg("page->setYLogScale() not supported");
        }
        | plot DP_Y_AXIS_FORMAT ':' DP_STR {
                //currPlot->setYAxisFormat($4);
                msg("page->setYAxisFormat() not supported");
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
                     currXVar = currCurve->setXVarName("");
                     currXVar->setUnit("--");
                     currXVar->setLabel("");
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
                currPlot->setXMinRange($4);
        }
        | x_var DP_MAX_RANGE ':' DP_FLOAT {
                currPlot->setXMaxRange($4);
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
                //currXVar->setTimeUnit($4);
                msg("plot->setTimeUnit() not supported");
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
                currPlot->setYMinRange($4);
        }
        | y_var DP_MAX_RANGE ':' DP_FLOAT {
                currPlot->setYMaxRange($4);
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
        | y_var DP_SYMBOL_SIZE ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairYVar->setSymbolSize($4);
                } else {
                    currYVar->setSymbolSize($4);
                }
        }
        | y_var DP_GNUPLOT_FUNCTION_STYLE ':' DP_STR {
                //currCurve->setGnuplotFunctionStyle($4);
                msg("plot->setGnuplotFunctionStyle() not supported");
        }
        | y_var DP_TIME_NAME ':' DP_STR {
                if ( isXYPair ) {
                    currXYPairYVar->setTimeName($4);
                } else {
                    currYVar->setTimeName($4);
                }
        }
        | y_var DP_TIME_UNITS ':' DP_STR {
                //currYVar->setTimeUnit($4);
                msg("plot->setTimeUnit() not supported");
        }
        | y_var DP_GNUPLOT_LINE_STYLE ':' DP_STR {
                //currCurve->setGnuplotLineStyle($4);
                msg("plot->setGnuplotLineStyle() not supported");
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
                //currCurve->setLabel($4);
                msg("plot->setLabel() not supported");
        }
        | curve DP_LINE_STYLE ':' DP_STR {
                //currCurve->setLineStyle($4);
                msg("plot->setLineStyle() not supported");
        }
        | curve DP_LINE_COLOR ':' DP_STR {
                currCurve->setLineColor($4);
        }
        | curve DP_SYMBOL_STYLE ':' DP_STR {
                //currCurve->setLineSymbol($4);
                msg("plot->setLineSymbol() not supported");
        }
        | curve DP_SYMBOL_SIZE ':' DP_STR {
                //currCurve->setLineSymbolSize($4);
                msg("plot->setLineSymbolSize() not supported");
        }
        | curve DP_GNUPLOT_FUNCTION_STYLE ':' DP_STR {
                //currCurve->setGnuplotFunctionStyle($4);
                msg("plot->setGnuplotFunctionStyle() not supported");
        }
        | curve DP_GNUPLOT_LINE_STYLE ':' DP_STR {
                //currCurve->setGnuplotLineStyle($4);
                msg("plot->setGnuplotLineStyle() not supported");
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
                //currTable->setChangeOnly($4);
        }
        | table_options DP_COLUMN_WIDTH ':' DP_FLOAT {
                //currTable->setColumnWidth((int)$4);
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


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
DPPage* currPage = 0;
DPPlot* currPlot = 0;
DPCurve* currCurve = 0;
DPVar* currXVar = 0;
DPVar* currYVar = 0;

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
                //product->getCurrPage()->setForegroundColor($4);
                msg("page->setGnuplotPageOrientation() not supported");
        }
        | page_options DP_BACKGROUND_COLOR ':' DP_STR  {
                //product->getCurrPage()->setBackgroundColor($4);
                msg("page->setGnuplotPageOrientation() not supported");
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
                //currPlot->setForegroundColor($4);
                msg("plot->setForegroundColor() not supported");
        }
        | plot DP_BACKGROUND_COLOR ':' DP_STR  {
                //currPlot->setBackgroundColor($4);
                msg("page->setBackgroundColor() not supported");
        }
        | plot DP_FONT ':' DP_STR  {
                //currPlot->setFont($4);
                msg("page->setBackgroundColor() not supported");
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
                //currPlot->setGrid($4);
                msg("page->setGrid() not supported");
        }
        | plot DP_GRID_COLOR ':' DP_STR  {
                //currPlot->setGridColor($4);
                msg("page->setGridColor() not supported");
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
                     currXVar = currCurve->setXVarName("sys.exec.out.time");
                     currXVar->setUnit("s");
                     currXVar->setLabel("time");
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
                currXVar = currCurve->setXVarName($3);
        }
        | x_var DP_LABEL ':' DP_STR {
                currXVar->setLabel($4);
        }
        | x_var DP_UNITS ':' DP_STR {
                currXVar->setUnit($4);
        }
        | x_var DP_MIN_RANGE ':' DP_FLOAT {
                //currXVar->setMinRange($4);
                msg("plot->setMinRange() not supported");
        }
        | x_var DP_MAX_RANGE ':' DP_FLOAT {
                //currXVar->setMaxRange($4);
                msg("plot->setMaxRange() not supported");
        }
        | x_var DP_SCALE_FACTOR ':' DP_FLOAT {
                //currXVar->setScaleFactor($4);
                msg("plot->setMinRange() not supported");
        }
        | x_var DP_BIAS ':' DP_FLOAT {
                //currXVar->setBias($4);
                msg("plot->setBias() not supported");
        }
        | x_var DP_TIME_NAME ':' DP_STR {
                //currXVar->setTimeName($4);
                msg("plot->setTimeName() not supported");
        }
        | x_var DP_TIME_UNITS ':' DP_STR {
                //currXVar->setTimeUnit($4);
                msg("plot->setTimeUnit() not supported");
        }
        ;

y_var: DP_Y_VARIABLE ':' DP_STR {
                // DP_STR is y variable name
                currYVar = currCurve->setYVarName($3);
        }
        | y_var DP_LABEL ':' DP_STR {
                currYVar->setLabel($4);
        }
        | y_var DP_UNITS ':' DP_STR {
                currYVar->setUnit($4);
        }
        | y_var DP_MIN_RANGE ':' DP_FLOAT  {
                //currYVar->setMinRange($4);
                msg("plot->setMinRange() not supported");
        }
        | y_var DP_MAX_RANGE ':' DP_FLOAT {
                //currYVar->setMaxRange($4);
                msg("plot->setMaxRange() not supported");
        }
        | y_var DP_SCALE_FACTOR ':' DP_FLOAT {
                //currYVar->setScaleFactor($4);
                msg("plot->setScaleFactor() not supported");
        }
        | y_var DP_BIAS ':' DP_FLOAT {
                //currYVar->setBias($4);
                msg("plot->setBias() not supported");
        }
        | y_var DP_LINE_STYLE ':' DP_STR {
                //currCurve->setLineStyle($4);
                msg("plot->setLineStyle() not supported");
        }
        | y_var DP_LINE_COLOR ':' DP_STR {
                //currCurve->setLineColor($4);
                msg("plot->setLineColor() not supported");
        }
        | y_var DP_SYMBOL_STYLE ':' DP_STR {
                //currCurve->setLineSymbol($4);
                msg("plot->setLineSymbol() not supported");
        }
        | y_var DP_SYMBOL_SIZE ':' DP_STR {
                //currCurve->setLineSymbolSize($4);
                msg("plot->setLineSymbolSiz() not supported");
        }
        | y_var DP_GNUPLOT_FUNCTION_STYLE ':' DP_STR {
                //currCurve->setGnuplotFunctionStyle($4);
                msg("plot->setGnuplotFunctionStyle() not supported");
        }
        | y_var DP_TIME_NAME ':' DP_STR {
                //currYVar->setTimeName($4);
                msg("plot->setTimeName() not supported");
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
        | curve DP_XY_PAIR ':' '{' y_var '}' {
                isXYPair = true;
                msg("plot->addXYPair() not supported");
        }
        | curve DP_XY_PAIR ':' '{' x_var y_var '}' {
                isXYPair = true;
                msg("plot->addXYPair() not supported");
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
                //currCurve->setLineColor($4);
                msg("plot->setLineColor() not supported");
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

table: DP_TABLE DP_FLOAT ':' DP_STR { 
                //product->addTable($4);
                msg("Tables not supported");
        }
        | table DP_START ':' DP_FLOAT {
                //product->getCurrTable()->setStartTime($4);
        }
        | table DP_STOP ':' DP_FLOAT {
                //product->getCurrTable()->setStopTime($4);
        }
        | table DP_CHANGE_ONLY ':' DP_STR {
                //product->getCurrTable()->setChangeOnly($4);
        }
        | table DP_COLUMN_WIDTH ':' DP_FLOAT {
                //product->getCurrTable()->setColumnWidth((int)$4);
        }
        | table DP_DELIMITER ':' DP_STR {
                //product->getCurrTable()->setDelimiter($4);
        }
        | table table_var
        ;
        
table_var: DP_VARIABLE ':' DP_STR {
                // DP_STR is variable name
                //product->addTableVar($3);
        }
        | table_var DP_LABEL ':' DP_STR {
                //product->getCurrTableVar()->setLabel($4);
        }
        | table_var DP_UNITS ':' DP_STR {
                ////product->getCurrTableVar()->setUnit($4);
        }
        | table_var DP_MIN_RANGE ':' DP_FLOAT {
                //product->getCurrTableVar()->setMinRange($4);
        }
        | table_var DP_MAX_RANGE ':' DP_FLOAT {
                //product->getCurrTableVar()->setMaxRange($4);
        }
        | table_var DP_SCALE_FACTOR ':' DP_FLOAT {
                //product->getCurrTableVar()->setScaleFactor($4);
        }
        | table_var DP_BIAS ':' DP_FLOAT {
                //product->getCurrTableVar()->setBias($4);
        }
        | table_var DP_FORMAT ':' DP_STR {
                //product->getCurrTableVar()->setFormat($4);
        }
        ;
%%

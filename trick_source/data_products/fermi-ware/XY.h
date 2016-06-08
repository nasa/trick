/*******************************************************************************
*									       *
* XY.h - General Purpose Plot Widget, Public Header File		       *
*									       *
* Copyright (c) 1991 Universities Research Association, Inc.		       *
* All rights reserved.							       *
* 									       *
* This material resulted from work developed under a Government Contract and   *
* is subject to the following license:  The Government retains a paid-up,      *
* nonexclusive, irrevocable worldwide license to reproduce, prepare derivative *
* works, perform publicly and display publicly by or for the Government,       *
* including the right to distribute to other Government contractors.  Neither  *
* the United States nor the United States Department of Energy, nor any of     *
* their employees, makes any warrenty, express or implied, or assumes any      *
* legal liability or responsibility for the accuracy, completeness, or         *
* usefulness of any information, apparatus, product, or process disclosed, or  *
* represents that its use would not infringe privately owned rights.           *
*                                        				       *
* Fermilab Nirvana GUI Library						       *
* May 28, 1992								       *
*									       *
* Written by Mark Edel							       *
*									       *
*******************************************************************************/

#ifndef  XY_H
#define  XY_H

enum XYRescaleModes {XY_NO_RESCALE, XY_RESCALE, XY_RESCALE_AT_MAX};
enum XYStringAlignments {XY_LEFT, XY_CENTER, XY_RIGHT};
#define XY_N_MARK_STYLES 10
#define XY_N_MARK_SIZES 4
#define XY_N_LINE_STYLES 13

/* JBF  added if _CURVE_INFO_ define stuff to match old versions and 
 * to be compatible w/Data Products
 */
#ifndef  _CURVE_INFO_
#define  _CURVE_INFO_
enum XYMarkStyles {XY_NO_MARK, XY_SQUARE_MARK, XY_CIRCLE_MARK, XY_STAR_MARK,
	XY_X_MARK, XY_TRIANGLE_MARK, XY_SOLID_SQUARE_MARK, XY_SOLID_CIRCLE_MARK,
	XY_THICK_SQUARE_MARK, XY_THICK_CIRCLE_MARK};
enum XYMarkSizes {XY_TINY, XY_SMALL, XY_MEDIUM, XY_LARGE};
enum XYLineStyles {XY_NO_LINE, XY_PLAIN_LINE, XY_FINE_DASH, XY_MED_FINE_DASH,
	XY_DASH, XY_LONG_DASH, XY_X_LONG_DASH, XY_1_DOT_DASH, XY_2_DOT_DASH,
	XY_3_DOT_DASH, XY_4_DOT_DASH, XY_THICK_LINE, XY_X_THICK_LINE};
#endif /* _CURVE_INFO_ */

/* Resource strings */
#define XmNdoubleBuffer "doubleBuffer"
#define XmCDoubleBuffer "DoubleBuffer"
#define XmNshowLegend "showLegend"
#define XmCShowLegend "ShowLegend"
#define XmNshowGrid "showGrid"
#define XmCShowGrid "ShowGrid"
#define XmNgridColor "gridColor"
#define XmCGridColor "gridColor"
#define XmNprintGridColor "printGridColor"
#define XmCPrintGridColor "printGridColor"
#define XmNxLogScaling "xLogScaling"
#define XmCXLogScaling "XLogScaling"
#define XmNyLogScaling "yLogScaling"
#define XmCYLogScaling "YLogScaling"
#define XmNmarginPercent "marginPercent"
#define XmCMarginPercent "MarginPercent"
#define XmNbtn2Callback "btn2Callback"
#define XmCBtn2Callback "Btn2Callback"
#define XmNbtn3Callback "btn3Callback"
#define XmCBtn3Callback "Btn3Callback"

/* JBF - added plotTitle from old version */
#define XmNplotTitle "plotTitle"
#define XmCPlotTitle "PlotTitle"

#define XmNxAxisLabel "xAxisLabel"
#define XmCXAxisLabel "XAxisLabel"
#define XmNyAxisLabel "yAxisLabel"
#define XmCYAxisLabel "YAxisLabel"
#define XmNredisplayCallback "redisplayCallback"
#define XmCRedisplayCallback "RedisplayCallback"

/* Keith added the crosshairs */
#define XmNenableCrosshairs "enableCrosshairs"
#define XmCEnableCrosshairs "EnableCrosshairs"

#define XmNValueDisplay "valueDisplay"
#define XmCValueDisplay "ValueDisplay"

#define XmNyAxisFormat "yAxisFormat"
#define XmCYAxisFormat "YAxisFormat"

extern WidgetClass xyWidgetClass;

typedef struct _XYClassRec *XYWidgetClass;
typedef struct _XYRec *XYWidget;

/*
 * It is possible that XYPoint will be defined in AppUtil.hh or somewhere else
 * This is my work around... :-( 
 */
#ifndef _XYPOINT_
#define _XYPOINT_
typedef struct _XYPoint {
    double x, y;
} XYPoint;
#endif

typedef struct _XYErrorBar {
    float min, max;
} XYErrorBar;

typedef struct _XYString {
    float x, y;
#if XmVERSION < 2
    XmFontList render_font;
#else
    XmRenderTable render_font;
#endif
    Pixel color;
    int alignment;
    XmString string;
} XYString;

typedef struct {
    int reason;
    XEvent *event;
} XYCallbackStruct;

typedef struct _XYCurve {
    XmString curve_id ;
    XmString name;
    char markerStyle;
    char markerSize;
    char lineStyle;
    Pixel markerPixel;
    Pixel linePixel;
    int nPoints;
    XYPoint *points;
    XYErrorBar *horizBars;
    XYErrorBar *vertBars;
    int flatline ;
    double flatlineValue ;
} XYCurve;

typedef struct {
    Widget x_formula ;
    Widget y_formula ;
    XYWidget w  ;
} XYManipulateCurveCBData ;

typedef struct {
    Widget   curves_formula ;
    XYWidget w  ;
} XYCombineCurvesCBData ;

void XYSetContents(Widget w, XYCurve *curves, int nCurves, int rescaleMode);
void XYUpdateStyles(Widget w, XYCurve *curves);
void XYUpdateData(Widget w, XYCurve *curves, int rescaleMode);
void XYSetStrings(Widget w, XYString *strings, int nStrings);
void XYSetVisibleRange(Widget w, double minXLim, double minYLim,
	double maxXLim, double maxYLim);
void XYGetVisibleRange(Widget w, double *minXLim, double *minYLim,
	double *maxXLim, double *maxYLim);
void XYZoomOut(Widget w);
void XYZoomIn(Widget w);
void XYResetZoom(Widget w);
void XYPrintContents(Widget w, char *psFileName);
/* JBF */
void XYPrintMultiple(Widget w[], 
                     int    landscape, 
                     int    num_plots, 
                     int    num_rows, 
                     int    num_columns, 
                     char   *page_title, 
                     char   *page_sub_title, 
                     Boolean DestroyOldFile,
                     char   *psFileName);

int getXYCurves(Widget w, XYCurve **curve) ;
int addXYPoints(Widget w, int curve_num, XYPoint *data, int num_points) ;
/* end JBF */
void XYDrawMarker(Display *display, Drawable drawBuf, GC gc, int size,
	int style, Pixel color, int x, int y);
void XYDrawLine(Display *display, Drawable drawBuf, GC gc, int style,
	Pixel color, int x1, int y1, int x2, int y2);
#endif

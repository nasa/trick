/*******************************************************************************
*     								       *
* XY.c -- General Purpose Plot Widget     				       *
*     								       *
* Copyright (c) 1991 Universities Research Association, Inc.     	       *
* All rights reserved.     						       *
*      								       *
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
*                                             			       *
* Fermilab Nirvana GUI Library     					       *
* May 28, 1992     							       *
*     								       *
* Written by Mark Edel     						       *
*     								       *
*******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <float.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/CoreP.h>
#include <X11/StringDefs.h>
#include <Xm/Xm.h>
#include <Xm/XmP.h>
#include <Xm/DialogS.h>
#include <Xm/PanedW.h>
#include <Xm/Form.h>
#include <Xm/LabelG.h>
#include <Xm/Text.h>
#include <Xm/PushBG.h>
#include <Xm/SelectioB.h>
#include <Xm/MessageB.h>
#if XmVersion == 1002
#include <Xm/PrimitiveP.h>
#endif
#include "psUtils.h"
#include "drawAxes.h"
#include "dragAxes.h"
#include "XYP.h"


#include "../EQParse/eqparse_protos.h"

#define REDRAW_NONE 0
#define REDRAW_H_AXIS 1
#define REDRAW_V_AXIS 2
#define REDRAW_CONTENTS 4
#define REDRAW_LABELS 8
#define REDRAW_ALL 15

#define ZOOM_FACTOR .25     	/* (linear) fraction of currently displayed
     			   data to place outside of current limits 
     			   when user invokes zoom command */
#define LEFT_MARGIN 0     	/* empty space to left of widget.  Should be
     			   small since v axis usually reserves more
     			   space than it needs (for long numbers) */
#define TOP_MARGIN 7     	/* empty space at top of widget */
#define RIGHT_MARGIN 0     	/* empty space to right of widget.  Should be
     			   small because h axis reserves more room
     			   than it needs for last label to stick out */
#define BOTTOM_MARGIN 3     	/* empty space at bottom of widget */
#define X_LABEL_MARGIN 7     /* space between x axis label and numbers */
#define Y_LABEL_MARGIN 15     /* space between y axis label and axis line */
#define LEGEND_TOP_MARGIN 5     /* space between legend and y axis label */
#define LEGEND_LEFT_MARGIN 5     /* space to the left of margin */
#define LEGEND_RIGHT_MARGIN 5     /* space to the right of margin */
#define LEGEND_LINE_LEN 40     /* width of example lines in plot legend */
#define LEGEND_NAME_SPACING 5     /* space between sample line and name */
#define LEGEND_COLUMN_MARGIN 20     /* space between columns in legend */
#define TINY_MARK_SIZE 2     /* width/height for small marker symbols */
#define SMALL_MARK_SIZE 4     /* width/height for small marker symbols */
#define MED_MARK_SIZE 6     	/* width/height for medium marker symbols */
#define LARGE_MARK_SIZE 8     /* width/height for large marker symbols */
#define ERROR_MARKER_RATIO 1.25     /* ratio of error bar end to marker size */

/* Set of constants for data to window coordinate transformation */
typedef struct {
    char xLogScaling, yLogScaling;
    double minXData, minYData, minXLim, minYLim, maxXLim, maxYLim;
    double xScale, yScale, minXPix, maxYPix;
} transform;

/* header of a list for accumulating figures (XRectangles, XArcs, XSegments)
   for drawing markers independent of their composition */
typedef struct {
    int style;
    int nFigures;
    char *list;
    char *fillPtr;
} markStruct;

/* header for a list for acumulating line segments in connected groups so that
   they can be drawn with a continuous dash pattern, and independent of their
   destination (FloatPoints for PSFloatDrawLines and XPoints for XDrawLines) */
typedef struct {
    int outDevice;
    XPoint *points;
    FloatPoint *floatPoints;
    int *lineCounts;
    int *curLineCount;
    XPoint *fillPtr;
    FloatPoint *fFillPtr;
} lineStruct;

typedef struct {
    XPoint dispPoint ;
    XYPoint value ;
} pointStruct;

/* JBF - added ZoomData from old version */
typedef struct
{
 int x1, y1, x2, y2;
} ZoomData;


/* legend is drawn differently to save space when user doesn't have
   lines and/or markers on any of the curves */
enum legendStyles {NO_MARKERS, NO_LINES, FULL_LEGEND};

void GetWidgetPopupList(Widget w, WidgetList *wl, int *num) ;

void errorEQParseDialog( Widget w, int error_code ) ;
int maxnumpts(XYCurve* curves, int numCurves) ;
XYCurve* curvemath( Widget parent_w, XYCurve* curves, 
                    int numCurves, char *equation) ;
XYCurve* curvesub(XYCurve* c1, XYCurve* c2) ;
static void setResourceGridColor( Widget w, int offset, XrmValue *value ) ;
void label_pointer( XYWidget w, double x_data, double y_data, char* txt ) ;
void getXYMousePosition( XYWidget w, XEvent *event, double* x, double* y ) ;
int curveClosestToMouseClick( XYWidget w, XEvent *event,
                              double* xOnCurve, double* yOnCurve ) ;
static void motion(XYWidget w, XEvent *event, char *args, int n_args) ;
static void crossHairs(XYWidget w, XEvent *event, char *args, int n_args) ;
static void valueDisplay(XYWidget w, XEvent *event, char *args, int n_args) ;
void drawCrosshairs( Display* display, Drawable drawBuf,
                     XYWidget w, int x_win, int y_win ) ;
static void selectCurve(XYWidget w, XEvent *event, char *args, int n_args);
void combineCurvesDialog(XYWidget w, XEvent *event, char* args, int n_args) ;
void combineCurvesOKCB( Widget w, XtPointer client_data, 
                        XtPointer call_data ) ;
void combineCurvesCancelCB( Widget w, XtPointer client_data, 
                            XtPointer call_data ) ;
void combineCurvesHelpCB(Widget w, XtPointer client_data, 
                         XtPointer call_data ) ;
void manipulateCurve(XYWidget w, XEvent *event, char* args, int n_args) ;
void manipulateCurveCB ( Widget w, XtPointer client_data, 
                         XtPointer call_data ) ;
static void motionAP(XYWidget w, XEvent *event, char *args, int n_args);
static void btnUpAP(XYWidget w, XEvent *event, char *args, int n_args);
/** JBF removed to match old and add zoom **
static void btn2AP(XYWidget w, XEvent *event, char *args, int n_args);
******/
/* JBF added *_zoom from old version ***/
static void start_zoom(XYWidget w, XEvent *event, char *args, int n_args);
static void track_zoom(XYWidget w, XEvent *event, char *args, int n_args);
static void end_zoom(XYWidget w, XEvent *event, char *args, int n_args);
/* end JBF */

static void btn3AP(XYWidget w, XEvent *event, char *args, int n_args);
static void initialize(XYWidget request, XYWidget new);
static void redisplay(XYWidget w, XEvent *event, Region region);
static void redisplayContents(XYWidget w, int outDevice, int redrawArea);
static void destroy(XYWidget w);
static void resize(XYWidget w);
static void resizeParts(XYWidget w);
static Boolean setValues(XYWidget current, XYWidget request,XYWidget new);
static void setContents(Widget w, XYCurve *curves, int nCurves, int rescaleMode,
     int dataOnly);
static int resetDataAndViewRanges(XYWidget w, int rescaleMode);
static void drawCurve(XYWidget w, Drawable drawBuf, int outDevice, XYCurve
     *curve);
static void startLineList(lineStruct *lineList, int outDevice, int maxSegs);
static void addToLineList(lineStruct *lineList, float x, float y, int restart,
     int round);
static void drawLineList(Display *display, Drawable drawBuf, GC gc,
     int outDevice, lineStruct *lineList, XYCurve *curve);
static void startMarkList(markStruct *markList, int markerStyle, int maxLength);
static void addToMarkList(markStruct *markList, int x, int y, int size);
static void drawMarkList(Display *display, Drawable drawBuf, GC gc, int outDevice,
     markStruct *markList, XYCurve *curve);
static int tryLegendLayout(XYWidget w, int nRows, int *width, int *nColumns);
static int legendColumnStyle(XYCurve *curves, int nCurves, int colNum,
     int nRows);
static void layoutLegend(XYWidget w, int *nRows, int *legendHeight,
     int *columnWidth, int *leftMargin);
static void drawLegend(XYWidget w, Drawable drawBuf, int outDevice);
static void drawMarker(Display *display, Drawable drawBuf, GC gc, int outDevice,
     XYCurve *curve, int x, int y);
static void drawLine(Display *display, Drawable drawBuf, GC gc, int outDevice,
     XYCurve *curve, int x1, int y1, int x2, int y2);
static void updateBufferAllocation(XYWidget w);
#if XmVERSION < 2
static XFontStruct *getFontStruct(XmFontList font);
#else
static void DefaultFont(Widget w, int offset, XrmValue *value);
#endif
static void calcDataRange(XYWidget w, double *xMin, double *xMax, double *yMin,
     double *yMax);
static void oneCurveDataRange(XYWidget w, XYCurve *curve, double *xMin,
     double *xMax, double *yMin, double *yMax);
static void computeTransform(XYWidget w, transform *xform);
static double dataToWindowX(transform *xform, double value);
static double dataToWindowY(transform *xform, double value);
static void copyCurveData(XYCurve *fromCurve, XYCurve *toCurve, int dataOnly);
static void copyCurveStyle(XYCurve *fromCurve, XYCurve *toCurve);
static void freeCurveData(XYCurve *curve, int dataOnly);
static double dMin(double d1, double d2);
static double dMax(double d1, double d2);
double dist_point_to_curve( XYCurve* curve, double x0, double y0, 
                            double* x_on_curve, double* y_on_curve ) ;

/* Line dash styles (defined as strings for convenience, but note that the
   numbers are in octal) */
static char *DashLists[] = {
    "\01" /* XY_NO_LINE (not used) */,
    "\01" /* XY_PLAIN_LINE (used w/dashes off) */,
    "\01\01" /* XY_FINE_DASH */,
    "\02\02" /* XY_MED_FINE_DASH */,
    "\04\04" /* XY_DASH */,
    "\06\06" /* XY_LONG_DASH */,
    "\016\06" /* XY_X_LONG_DASH */,
    "\020\03\02\03" /* XY_1_DOT_DASH */,
    "\016\03\02\03\02\03" /* XY_2_DOT_DASH */,
    "\013\03\02\03\02\03\02\03" /* XY_3_DOT_DASH */,
    "\011\03\02\03\02\03\02\03\02\03" /* XY_4_DOT_DASH */,
    "\01" /* XY_THICK_LINE (used w/dashes off) */,
    "\01" /* XY_X_THICK_LINE (used w/dashes off) */
};

/* Table to translate mark width index to actual width in pixels */
static char MarkWidths[] = {TINY_MARK_SIZE, SMALL_MARK_SIZE,
     MED_MARK_SIZE, LARGE_MARK_SIZE};

/* JBF - added Btn2Motion and Btn2Up from old version */

static char defaultTranslations[] = 
    "~Ctrl Shift<Btn1Down>: manipulateCurve()\n\
     ~Shift Ctrl<Btn1Down>: combineCurvesDialog()\n\
     Shift Ctrl<Btn1Down>: selectCurve()\n\
     <Btn1Motion>: Motion()\n\
     <Btn1Down>: Motion()\n\
     <Btn1Up>: BtnUp()\n\
     <Btn2Down>: Btn2Down()\n\
     <Btn2Motion>: Btn2Motion()\n\
     <Btn2Up>: Btn2Up()\n\
     <Btn3Down>: Btn3Press()\n\
     <Motion>: motion()\n" ;

/* JBF - added Btn2Down, Btn2Motion, and Btn2Up zoom stuff from old version,
         and removed Btn2Press */
static XtActionsRec actionsList[] = {
    {"selectCurve", (XtActionProc)selectCurve},
    {"manipulateCurve",     (XtActionProc)manipulateCurve},
    {"combineCurvesDialog", (XtActionProc)combineCurvesDialog},
    {"Motion", (XtActionProc)motionAP},
    {"BtnUp", (XtActionProc)btnUpAP},
    {"Btn2Down", (XtActionProc)start_zoom},
    {"Btn2Motion", (XtActionProc)track_zoom},
    {"Btn2Up", (XtActionProc)end_zoom},
    {"Btn3Press", (XtActionProc)btn3AP},
    {"motion", (XtActionProc)motion}
};

static XtResource resources[] = {
/* Keith added cross hairs */
    {XmNenableCrosshairs, XmCEnableCrosshairs, XmRBoolean, sizeof(Boolean),
      XtOffset(XYWidget, xy.enableCrosshairs), XmRString, "False"},

    {XmNdoubleBuffer, XmCDoubleBuffer, XmRBoolean, sizeof(Boolean),
      XtOffset(XYWidget, xy.doubleBuffer), XmRString, "False"},
    {XmNshowLegend, XmCShowLegend, XmRBoolean, sizeof(Boolean),
      XtOffset(XYWidget, xy.showLegend), XmRString, "True"},
/* RCS added grid on/off stuff
*/
    {XmNshowGrid, XmCShowGrid, XmRBoolean, sizeof(Boolean),
      XtOffset(XYWidget, xy.showGrid), XmRString, "True"},
    {XmNgridColor, XmCGridColor, XmRPixel, sizeof (Pixel),
      XtOffset(XYWidget, xy.gridColor), XtRCallProc, 
      (XtPointer) setResourceGridColor},
    {XmNprintGridColor, XmCPrintGridColor, XmRPixel, sizeof (Pixel),
      XtOffset(XYWidget, xy.printGridColor), XmRPixel, 0},
    {XmNxLogScaling, XmCXLogScaling, XmRBoolean, sizeof(Boolean),
      XtOffset(XYWidget, xy.xLogScaling), XmRString, "False"},
    {XmNyLogScaling, XmCYLogScaling, XmRBoolean, sizeof(Boolean),
      XtOffset(XYWidget, xy.yLogScaling), XmRString, "False"},
    {XmNmarginPercent, XmCMarginPercent, XmRInt, sizeof(int),
      XtOffset(XYWidget, xy.marginPercent), XmRString, "4"},

/* JBF - added plotTitle from old version */
    {XmNplotTitle, XmCPlotTitle, XmRXmString, sizeof (XmString),
      XtOffset(XYWidget, xy.plotTitle), XmRString, NULL},

#if XmVERSION < 2
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
      XtOffset(XYWidget, xy.render_font), XmRImmediate, NULL},
#else
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
      XtOffset(XYWidget, xy.render_font), XmRCallProc, (XtPointer)DefaultFont},
#endif
    {XmNxAxisLabel, XmCXAxisLabel, XmRXmString, sizeof (XmString), 
      XtOffset(XYWidget, xy.xAxisLabel), XmRString, NULL},
    {XmNyAxisLabel, XmCYAxisLabel, XmRXmString, sizeof (XmString), 
      XtOffset(XYWidget, xy.yAxisLabel), XmRString, NULL},
    {XmNresizeCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
      XtOffset (XYWidget, xy.resize), XtRCallback, NULL},
    {XmNbtn2Callback, XmCCallback, XmRCallback, sizeof(caddr_t),
      XtOffset (XYWidget, xy.btn2), XtRCallback, NULL},
    {XmNbtn3Callback, XmCCallback, XmRCallback, sizeof(caddr_t),
      XtOffset (XYWidget, xy.btn3), XtRCallback, NULL},
    {XmNredisplayCallback, XmCCallback, XmRCallback, sizeof(caddr_t),
      XtOffset (XYWidget, xy.redisplay), XtRCallback, NULL},
    {XmNyAxisFormat, XmCYAxisFormat, XmRString, sizeof(XmString),
      XtOffset(XYWidget, xy.yAxisFormat), XmRString, NULL},

    {XmNValueDisplay, XmCValueDisplay, XmRBoolean, sizeof(Boolean),
      XtOffset(XYWidget, xy.valueDisplay), XmRString, "False"},

};

XYClassRec  xyClassRec = {
     /* CoreClassPart */
  {
    (WidgetClass) &xmPrimitiveClassRec,  /* superclass       */
    "XY",                         /* class_name            */
    sizeof(XYRec),                /* widget_size           */
    NULL,                           /* class_initialize      */
    NULL,                           /* class_part_initialize */
    FALSE,                          /* class_inited          */
    (XtInitProc)initialize,         /* initialize            */
    NULL,                           /* initialize_hook       */
    XtInheritRealize,               /* realize               */
    actionsList,                    /* actions               */
    XtNumber(actionsList),          /* num_actions           */
    resources,                      /* resources             */
    XtNumber(resources),            /* num_resources         */
    NULLQUARK,                      /* xrm_class             */
    TRUE,                           /* compress_motion       */
    TRUE,                           /* compress_exposure     */
    TRUE,                           /* compress_enterleave   */
    TRUE,                           /* visible_interest      */
    (XtWidgetProc)destroy,          /* destroy               */
    (XtWidgetProc)resize,           /* resize                */
    (XtExposeProc)redisplay,        /* expose                */
    (XtSetValuesFunc)setValues,     /* set_values            */
    NULL,                           /* set_values_hook       */
    XtInheritSetValuesAlmost,       /* set_values_almost     */
    NULL,                           /* get_values_hook       */
    NULL,                           /* accept_focus          */
    XtVersion,                      /* version               */
    NULL,                           /* callback private      */
    defaultTranslations,            /* tm_table              */
    NULL,                           /* query_geometry        */
    NULL,                           /* display_accelerator   */
    NULL,                           /* extension             */
  },
  /* Motif primitive class fields */
  {
     (XtWidgetProc)_XtInherit,        /* Primitive border_highlight   */
     (XtWidgetProc)_XtInherit,        /* Primitive border_unhighlight */
     XtInheritTranslations,     	/* translations                 */
    (XtActionProc)motionAP,     	/* arm_and_activate             */
     NULL,     			/* get resources      		*/
     0,     				/* num get_resources  		*/
     NULL,              		/* extension                    */
  },
  /* XY class part */
  {
    0,                                   /* ignored	                */
  }
};

/* JBF added zd from old version */
static ZoomData zd;


WidgetClass xyWidgetClass = (WidgetClass)&xyClassRec;

static void setResourceGridColor( Widget w, int offset __attribute__ ((unused)), XrmValue *value ) {

     Display *display ; 
     XColor xWindowsColorScreen ;
     XColor xWindowsColorExact ;
     Colormap colormap ;
     Screen *screen ;

     display  = XtDisplay(w);
     screen   = XtScreen(w) ;
     colormap = DefaultColormapOfScreen(screen);
     XAllocNamedColor(display, colormap, "gray87",
                      &xWindowsColorScreen,
                      &xWindowsColorExact ) ;
     value->addr = (caddr_t) &xWindowsColorScreen.pixel ;
}

/*
** Widget initialize method
*/
static void initialize(XYWidget request, XYWidget new)
{
    XGCValues values;
    Display *display = XtDisplay(new);
#if XmVERSION >= 2
    XFontStruct *fs;
#endif

    /* Make sure the window size is not zero. The Core 
       initialize() method doesn't do this. */
    if (request->core.width == 0)
         new->core.width = 500;
    if (request->core.height == 0)
        new->core.height = 400;

    /* Make a local copy of the fontlist,
       or get the default if not specified */
#if XmVERSION < 2
    if (new->xy.render_font == NULL)
         new->xy.render_font =
             XmFontListCopy(_XmGetDefaultFontList(
             	    (Widget) new, XmLABEL_FONTLIST));
    else
        new->xy.render_font = XmFontListCopy(new->xy.render_font);
#else
    if (new->xy.render_font == NULL)
       new->xy.render_font =
            XmeGetDefaultRenderTable((Widget)new,XmTEXT_RENDER_TABLE);
    new->xy.render_font = XmRenderTableCopy(new->xy.render_font,NULL,0);
#endif

    /* Make local copies of the XmStrings */
/* JBF - added plotTitle from old version */
    if (new->xy.plotTitle != NULL)
        new->xy.plotTitle = XmStringCopy(new->xy.plotTitle);

    if (new->xy.xAxisLabel != NULL)
         new->xy.xAxisLabel = XmStringCopy(new->xy.xAxisLabel);
    if (new->xy.yAxisLabel != NULL)
         new->xy.yAxisLabel = XmStringCopy(new->xy.yAxisLabel);
     
    /* Create graphics contexts for drawing in the widget */
#if XmVERSION < 2
    values.font = getFontStruct(new->xy.render_font)->fid;
#else
    XmeRenderTableGetDefaultFont(new->xy.render_font,&fs);
    values.font = fs->fid;
#endif
    values.foreground = new->primitive.foreground;
    values.background = new->core.background_pixel;
    new->xy.gc = XCreateGC(display, XDefaultRootWindow(display),
             GCForeground|GCBackground|GCFont, &values);
    new->xy.contentsGC = XCreateGC(display, XDefaultRootWindow(display),
             GCForeground|GCBackground|GCFont, &values);
    
    /* Initialize various fields */
    ResetAxisDragging(&new->xy.dragState);
    new->xy.curves = NULL;
    new->xy.nCurves = 0;
    new->xy.strings = NULL;
    new->xy.nStrings = 0;
    new->xy.drawBuffer = 0;
    new->xy.valueBuffer = 0;
    new->xy.xEnd = 0;
    new->xy.xOrigin = 0;
    new->xy.yOrigin = 0;
    new->xy.yEnd = 0;
    new->xy.yAxisFormat = XmStringCreateLocalized("%g");
    
    /* Default plotting boundaries */
    setContents((Widget)new, NULL, 0, XY_RESCALE, False);

    /* JBF - added from previous version */
    values.foreground = new->primitive.foreground ^ new->core.background_pixel;
    values.line_style = LineSolid;
    values.function = GXxor ;
    new->xy.zoomGC = XtGetGC((Widget)new, 
                        GCForeground | GCBackground | GCLineStyle | GCFunction, 
                        &values);

    /* end JBF */

    /* Keith added cross hairs */
    new->xy.enableCrosshairs  = 0 ;      /* Default is not using them */
    new->xy.crosshairFirst = 1 ;

    new->xy.valueDisplay  = 0 ;      /* Default is not using them */

    /* Set size dependent items */
    resize(new);
}

/*
** Widget destroy method
*/
static void destroy(XYWidget w)
{
    int i;
    
    XFreeGC(XtDisplay(w), w->xy.gc);
    XFreeGC(XtDisplay(w), w->xy.contentsGC);
#if XmVERSION < 2
    if (w->xy.render_font != NULL)
         XmFontListFree(w->xy.render_font);
#else
    if (w->xy.render_font != NULL)
       XmRenderTableFree(w->xy.render_font);
#endif
    if (w->xy.drawBuffer)
         XFreePixmap(XtDisplay(w), w->xy.drawBuffer);
    if (w->xy.valueBuffer)
         XFreePixmap(XtDisplay(w), w->xy.valueBuffer);
    XtRemoveAllCallbacks ((Widget)w, XmNresizeCallback);
    XtRemoveAllCallbacks ((Widget)w, XmNbtn2Callback);
    XtRemoveAllCallbacks ((Widget)w, XmNbtn3Callback);
    XtRemoveAllCallbacks ((Widget)w, XmNredisplayCallback);

/* JBF - added plotTitle from old version */
    if (w->xy.plotTitle != NULL)
        XmStringFree(w->xy.plotTitle);

    if (w->xy.xAxisLabel != NULL)
         XmStringFree(w->xy.xAxisLabel);
    if (w->xy.yAxisLabel != NULL)
         XmStringFree(w->xy.yAxisLabel);
    if (w->xy.curves != NULL) {
         for (i=0; i<w->xy.nCurves; i++)
             freeCurveData(&w->xy.curves[i], False);
         XtFree((char *) w->xy.curves);
    }
    for (i=0; i<w->xy.nStrings; i++) {
#if XmVERSION < 2
         XmFontListFree(w->xy.strings[i].render_font);
#else
        XmRenderTableFree(w->xy.strings[i].render_font);
#endif
         XmStringFree(w->xy.strings[i].string);
    }
    if (w->xy.nStrings != 0)
         XtFree((char *)w->xy.strings);
}

/*
** Widget resize method.  Called when the total size of the widget changes
*/
static void resize(XYWidget w)
{
    /* resize the drawing buffer, an offscreen pixmap for smoother animation */
    updateBufferAllocation(w);
    
    /* change size dependent values in the widget */
    resizeParts(w);
   
    /* call the resize callback */
    if (XtIsRealized((Widget)w))
         XtCallCallbacks((Widget)w, XmNresizeCallback, NULL);
}

/*
** Internal resize procedure, called when the window is resized, or when
** something within the widget changes size (such as when labels are added or
** removed, or the legend changes size).  Re-calculates the sizes of the
** various part of the widget (highlights, shadows, axis labels, plot
** contents, plot legend, margins, etc.) and the size dependent elements
** in the widget instance data structure.
*/
static void resizeParts(XYWidget w)
{
    int legendHeight, borderWidth =
             w->primitive.shadow_thickness + w->primitive.highlight_thickness;
    XRectangle clipRect;
#if XmVERSION < 2
    XFontStruct *fs = getFontStruct(w->xy.render_font);
#else
    XFontStruct *fs;
    XmeRenderTableGetDefaultFont(w->xy.render_font,&fs);
#endif

    /* calculate the area of the widget where contents can be drawn */
    w->xy.xMin = borderWidth;
    w->xy.yMin = borderWidth;
    w->xy.xMax = w->core.width - borderWidth;
    w->xy.yMax = w->core.height - borderWidth;

    /* calculate how the space within the drawable area should be divided
       horizontally (layoutLegend below uses this information) */
    w->xy.axisLeft = w->xy.xMin + LEFT_MARGIN;
    w->xy.xOrigin = w->xy.axisLeft + VAxisWidth(fs, w->xy.yAxisFormat);
    w->xy.axisRight = w->xy.xMax - RIGHT_MARGIN;
    w->xy.xEnd = w->xy.axisRight - HAxisEndClearance(fs);

    /* find out how high the legend will need to be and how many rows it has */
    layoutLegend(w, &w->xy.legendRows, &legendHeight,
             &w->xy.legendColumnSpacing, &w->xy.legendLeftMargin);
    
    /* calculate how the space within the drawable area should be divided
       vertically, depending the sizes of the labels and plot legend */
    w->xy.legendTop = w->xy.yMax - BOTTOM_MARGIN - legendHeight;
    if ((w->xy.yAxisLabel != NULL) || (w->xy.plotTitle != NULL))
        w->xy.yEnd = w->xy.yMin + 3*(fs->ascent + fs->descent) + TOP_MARGIN;
    else
         w->xy.yEnd = VAxisEndClearance(fs) + fs->ascent/2 + TOP_MARGIN;
    w->xy.axisTop = w->xy.yEnd - VAxisEndClearance(fs);

    w->xy.axisBottom = w->xy.yMax - BOTTOM_MARGIN - legendHeight -
             (legendHeight == 0 ? 0 : LEGEND_TOP_MARGIN) -
             (w->xy.xAxisLabel == NULL ? fs->ascent/2 :
              fs->ascent + fs->descent + X_LABEL_MARGIN);
    w->xy.yOrigin = w->xy.axisBottom - HAxisHeight(fs);
    
    /* set plot contents gc to clip drawing at the edges */
    clipRect.x = w->xy.xOrigin;
    clipRect.y = w->xy.yEnd;
    clipRect.width = w->xy.xEnd - w->xy.xOrigin;
    clipRect.height = w->xy.yOrigin - w->xy.yEnd;
    XSetClipRectangles(XtDisplay(w), w->xy.contentsGC, 0, 0, &clipRect,
             1, Unsorted);

    /* set drawing gc to clip drawing before motif shadow and highlight */
    clipRect.x = borderWidth;
    clipRect.y = borderWidth;
    clipRect.width = w->core.width - 2 * borderWidth;
    clipRect.height = w->core.height - 2 * borderWidth;
    XSetClipRectangles(XtDisplay(w), w->xy.gc, 0, 0, &clipRect, 1, Unsorted);
}

/*
** Widget redisplay method
*/
static void redisplay(XYWidget w, XEvent *event __attribute__ ((unused)), Region region __attribute__ ((unused)))
{
    /* Draw the Motif required shadows and highlights */
    if (w->primitive.shadow_thickness > 0) {
     _XmDrawShadow (XtDisplay(w), XtWindow(w), w->primitive.bottom_shadow_GC,
     	w->primitive.top_shadow_GC, w->primitive.shadow_thickness,
     	w->primitive.highlight_thickness,
     	w->primitive.highlight_thickness,
     	w->core.width - 2 * w->primitive.highlight_thickness,
     	w->core.height-2 * w->primitive.highlight_thickness);
    }
    if (w->primitive.highlighted)
     _XmHighlightBorder((Widget)w);
    else if (_XmDifferentBackground((Widget)w, XtParent((Widget)w)))
     _XmUnhighlightBorder((Widget)w);
    
    /* Now draw the contents of the xy widget */
    redisplayContents(w, X_SCREEN, REDRAW_ALL);
}

/*
** Widget setValues method
*/
static Boolean setValues(XYWidget current, XYWidget request __attribute__ ((unused)), XYWidget new)
{
    Boolean redraw = False, doResize = False;
    Display *display = XtDisplay(new);

    /* If the background color has changed, change the GCs */
    if (new->core.background_pixel !=current->core.background_pixel) {
         XSetBackground(display, new->xy.gc, new->core.background_pixel);
         XSetBackground(display,new->xy.contentsGC,new->core.background_pixel);
         redraw = TRUE;
    }
    
    /* if the foreground color has changed, redraw */
    if (new->primitive.foreground != current->primitive.foreground)
         redraw = TRUE;

    /* if double buffering changes, allocate or deallocate offscreen pixmap */
    if (new->xy.doubleBuffer != current->xy.doubleBuffer) {
         updateBufferAllocation(new);
         redraw = TRUE;
    }

    /* if legend is turned on or off, resize everything */
    if (new->xy.showLegend != current->xy.showLegend)
         doResize = TRUE;

/* RCS if turning grid on or off need to redraw
*/
    if (new->xy.showGrid != current->xy.showGrid)
        redraw = TRUE;

    /* if log scaling changes, re-calculate data range if the plot has a
       margin, verify data (and set back to linear if data can't handle
       log scaling), reset dragging, and redraw */
    if   (new->xy.xLogScaling != current->xy.xLogScaling ||
           new->xy.yLogScaling != current->xy.yLogScaling) {
         if (new->xy.marginPercent == 0) {
         if (new->xy.xLogScaling && new->xy.minXData <= 0.)
     	new->xy.xLogScaling = False;
         if (new->xy.yLogScaling && new->xy.minYData <= 0.)
     	new->xy.yLogScaling = False;
         } else
             resetDataAndViewRanges((XYWidget)new, XY_RESCALE);
         ResetAxisDragging(&new->xy.dragState);
         redraw = TRUE;
    }

    /* if labels are changed, free the old ones and copy the new ones */
/* JBF - added plotTitle from old version */
    /* if labels are changed, free the old ones and copy the new ones */
    if (new->xy.plotTitle != current->xy.plotTitle) {
        if (current->xy.plotTitle != NULL)
            XmStringFree(current->xy.plotTitle);
        new->xy.plotTitle = XmStringCopy(new->xy.plotTitle);
        doResize = TRUE;
    }

    if (new->xy.xAxisLabel != current->xy.xAxisLabel) {
         if (current->xy.xAxisLabel != NULL)
             XmStringFree(current->xy.xAxisLabel);
         new->xy.xAxisLabel = XmStringCopy(new->xy.xAxisLabel);
         doResize = TRUE;
    }
    if (new->xy.yAxisLabel != current->xy.yAxisLabel) {
         if (current->xy.yAxisLabel != NULL)
             XmStringFree(current->xy.yAxisLabel);
         new->xy.yAxisLabel = XmStringCopy(new->xy.yAxisLabel);
         doResize = TRUE;
    }
    
    /* if highlight thickness or shadow thickness changed, resize and redraw */
    if  ((new->primitive.highlight_thickness != 
             current->primitive.highlight_thickness) ||
         (new->primitive.shadow_thickness !=
             current->primitive.shadow_thickness)) {
         doResize = TRUE;
    }
    if (doResize) {
         resize(new);
         return TRUE;
    }
    return redraw; 
} 


/*
** JBF - added *_zoom and set_lines from old version
**
**
** Button 2
** Button press and button motion and button release action procs for zoom.
*/

/*
** support routine for button 2 *_zoom action routines.
*/
static void set_lines(ZoomData data, XPoint *points)
{
        points[0].x = points[1].x = points[4].x = data.x1;
        points[0].y = points[3].y = points[4].y = data.y1;
        points[1].y = points[2].y = data.y2;
        points[2].x = points[3].x = data.x2;
}
 
static void start_zoom(XYWidget w, XEvent *event, char *args __attribute__ ((unused)), int n_args __attribute__ ((unused)))
{
XPoint points[5];
 
 
        zd.x1 = zd.x2 = event->xbutton.x;
        zd.y1 = zd.y2 = event->xbutton.y;
        set_lines(zd, points);
        XDrawLines(XtDisplay(w), XtWindow(w), w->xy.zoomGC,
                                points, 5, CoordModeOrigin);
 
}
static void track_zoom(XYWidget w, XEvent *event, char *args __attribute__ ((unused)), int n_args __attribute__ ((unused)))
{
XPoint points[5];
 
        set_lines(zd, points);
        XDrawLines(XtDisplay(w), XtWindow(w), w->xy.zoomGC,
                                points, 5, CoordModeOrigin);
 
        zd.x2 = event->xbutton.x;
        zd.y2 = event->xbutton.y;
        set_lines(zd, points);
        XDrawLines(XtDisplay(w), XtWindow(w), w->xy.zoomGC,
                                points, 5, CoordModeOrigin);
 
}
static void end_zoom(XYWidget w, XEvent *event __attribute__ ((unused)), char *args __attribute__ ((unused)), int n_args __attribute__ ((unused)))
{
XPoint points[5];
double minXLim, minYLim, maxXLim, maxYLim;
double xScale, yScale;
int x1, y11, x2, y2;
 
 
    set_lines( zd, points );
    XDrawLines( XtDisplay(w), XtWindow(w), w->xy.zoomGC,
          points, 5, CoordModeOrigin );
    if ( abs(zd.x2 - zd.x1) < 5 || abs(zd.y2 - zd.y1) < 5 )
    {
        XBell( XtDisplay( w ), 10 );
        return;  /* the magnifying glass is too small */
    }
 
        if (zd.x1 < zd.x2)
        {
                x1 = zd.x1;
                x2 = zd.x2;
        }
        else
        {
                x2 = zd.x1;
                x1 = zd.x2;
        }
        if (zd.y1 > zd.y2)
        {
                y11 = zd.y1;
                y2 = zd.y2;
        }
        else
        {
                y2 = zd.y1;
                y11 = zd.y2;
        }
 
        minXLim = w->xy.minXLim; maxXLim = w->xy.maxXLim;
        minYLim = w->xy.minYLim; maxYLim = w->xy.maxYLim;
 
        xScale = (maxXLim - minXLim)/(w->xy.xEnd - w->xy.xOrigin);
        yScale = (maxYLim - minYLim)/(w->xy.yOrigin - w->xy.yEnd);
 
        /* TOP */
        w->xy.maxYLim = minYLim + (double)(w->xy.yOrigin - y2) * yScale;
 
        /* BOTTOM */
        w->xy.minYLim = minYLim + (double)(w->xy.yOrigin - y11) * yScale;
 
        /* RIGHT */
        w->xy.maxXLim = minXLim + (double)(x2 - w->xy.xOrigin) * xScale;
 
        /* LEFT */
        w->xy.minXLim = minXLim + (double)(x1 - w->xy.xOrigin) * xScale;
 

        redisplayContents(w, X_SCREEN, REDRAW_ALL);
}


/*
** end Button 2 press, motion, and release action procs.
*/

/*
** Button press and button motion action proc.
*/
static void motionAP(XYWidget w, XEvent *event, char *args __attribute__ ((unused)), int n_args __attribute__ ((unused)))
{
    int chgdArea, redrawArea = REDRAW_NONE;
                                                                                
    if (event->type == ButtonPress)
       XmProcessTraversal((Widget)w, XmTRAVERSE_CURRENT);
                                                                                
    chgdArea = DragAxes(event, w->xy.xOrigin, w->xy.xEnd, w->xy.yOrigin,
       w->xy.yEnd, w->xy.axisLeft, w->xy.axisTop, w->xy.axisBottom,
       w->xy.axisRight, w->xy.minXData, w->xy.maxXData, w->xy.minYData,
       w->xy.maxYData, w->xy.xLogScaling, w->xy.yLogScaling,
       &w->xy.minXLim, &w->xy.maxXLim, &w->xy.minYLim, &w->xy.maxYLim,
       &w->xy.dragState, &w->xy.xDragStart, &w->xy.yDragStart);
    if (chgdArea & DA_REDRAW_H_AXIS) redrawArea |= REDRAW_H_AXIS;
    if (chgdArea & DA_REDRAW_V_AXIS) redrawArea |= REDRAW_V_AXIS;
    if (chgdArea & DA_REDRAW_CONTENTS) redrawArea |= REDRAW_CONTENTS;
                                                                                
    redisplayContents(w, X_SCREEN, redrawArea);
}

/*
** Shift-Button1-Down Selects A Curve
** Keith's Addition
** 7-11-2003 
*/
static void selectCurve(XYWidget w, XEvent *event, char *args __attribute__ ((unused)), int n_args __attribute__ ((unused)))
{
     int ii ;
     char* run_id ;
     int closest_curve_to_mouse ;
     double xOnCurve, yOnCurve ;
     Drawable drawBuf;
     Display *display = XtDisplay(w);

     closest_curve_to_mouse = curveClosestToMouseClick( w, event, 
                                                        &xOnCurve, &yOnCurve ) ;

     if (w->xy.doubleBuffer) {
          drawBuf = w->xy.drawBuffer;
     } else {
          drawBuf = XtWindow(w);
     } 
 
     /* Highlight selected curve and dehighlight last selected curve */
     for ( ii = 0 ; ii < w->xy.nCurves ; ii++ ) {
          if ( w->xy.curves[ii].lineStyle == XY_X_THICK_LINE ) {
               w->xy.curves[ii].lineStyle = XY_PLAIN_LINE ;
               drawCurve(w, drawBuf, X_SCREEN, &w->xy.curves[ii]);
          }
     }
     w->xy.curves[closest_curve_to_mouse].lineStyle = XY_X_THICK_LINE ;

     redisplayContents((XYWidget)w, X_SCREEN, REDRAW_ALL);
     drawCurve(w, drawBuf, X_SCREEN, &w->xy.curves[closest_curve_to_mouse]);

     /* Do this after redisplayContents() so it 
      * will be more visible on top of other the curves
      */
     if (w->xy.doubleBuffer ) {
          XCopyArea(display, drawBuf, XtWindow(w), w->xy.gc, 0, 0,
                    w->core.width, w->core.height, 0, 0);
     }
     
     if (XtIsRealized((Widget)w)) {
          XtCallCallbacks((Widget)w, XmNredisplayCallback, NULL);
     }

     /* Show curve number for selected curve */
     XmStringGetLtoR( w->xy.curves[closest_curve_to_mouse].curve_id,
                      XmFONTLIST_DEFAULT_TAG,
                      &run_id ) ; 
     label_pointer( w, xOnCurve, yOnCurve, run_id ) ;
}

/*
** Returns curve number closest to mouse click
** Also gives the x-y location on curve (not pixel location)
** Keith's Addition
** 7-11-2003 
*/
int curveClosestToMouseClick( XYWidget w, XEvent *event,
                              double* xOnCurve, double* yOnCurve ) {

     int ii ;
     double xMousePos, yMousePos ;
     int closest_curve_to_mouse ;
     double dist, min_dist ;
     double x, y ;

     getXYMousePosition( w, event, &xMousePos, &yMousePos ) ;  

     min_dist = 1.0e20 ;
     closest_curve_to_mouse = 0 ;
     for ( ii = 0 ; ii < w->xy.nCurves ; ii++ ) {
          dist = dist_point_to_curve( &(w->xy.curves[ii]), 
                                      xMousePos, yMousePos, 
                                      &x, &y ) ;
          if ( dist < min_dist ) {
               min_dist = dist ;
               closest_curve_to_mouse = ii ;
               *xOnCurve = x ;
               *yOnCurve = y ;
          }
     }

     return( closest_curve_to_mouse ) ;
}


/*
** Get Current x-y location of mouse click
** Location is not in pixel coords.  It is in data coords.
** Keith's addition
** 7-11-2003
*/
void getXYMousePosition( XYWidget w, XEvent *event, double* x, double* y ) {

     ResetAxisDragging(&w->xy.dragState) ;

     DragAxes(event, w->xy.xOrigin, w->xy.xEnd, w->xy.yOrigin,
              w->xy.yEnd, w->xy.axisLeft, w->xy.axisTop, w->xy.axisBottom,
              w->xy.axisRight, w->xy.minXData, w->xy.maxXData, w->xy.minYData,
              w->xy.maxYData, w->xy.xLogScaling, w->xy.yLogScaling,
              &w->xy.minXLim, &w->xy.maxXLim, &w->xy.minYLim, &w->xy.maxYLim,
              &w->xy.dragState, &w->xy.xDragStart, &w->xy.yDragStart);

     *x = w->xy.xDragStart ;
     *y = w->xy.yDragStart ;
}

/*
** Button up action proc.
*/
static void btnUpAP(XYWidget w, XEvent *event __attribute__ ((unused)), char *args __attribute__ ((unused)), int n_args __attribute__ ((unused)))
{
    ResetAxisDragging(&w->xy.dragState);
}

static void btn3AP(XYWidget w, XEvent *event __attribute__ ((unused)), char *args __attribute__ ((unused)), int n_args __attribute__ ((unused)))
{    

#ifdef ORIGINAL
    XYCallbackStruct cbStruct;
    XmProcessTraversal((Widget)w, XmTRAVERSE_CURRENT);

    /* Just call the callback */
    cbStruct.reason = XmCR_INPUT;
    cbStruct.event = event;
    XtCallCallbacks((Widget)w, XmNbtn3Callback, (XtPointer)&cbStruct);
#endif /* ORIGINAL */

/* JBF - copied stuff below from old version */

    resetDataAndViewRanges(w, XY_RESCALE);
    if (XtIsRealized((Widget)w))
       redisplayContents(w, X_SCREEN, REDRAW_ALL);

}

/*
** XYSetContents
**
** Specify the contents of the xy widget in the form of sets of points in 2D
** space.  Each set has a distinct marker and/or connecting line style, and
** possibly associated error bars (horizontal and/or vertical).  The data and
** appearance information for each set is called a curve.  XYSetContents
** takes an array of XYCurve data structures which contain all of this data,
** and passes it along to the XY widget to display.
**
** Parameters
**
**      w		An xy widget
**     curves		An array of curves to display
**     nCurves		The number of curves specified in curves
**     rescaleMode	One of: XY_NO_RESCALE, XY_RESCALE, or
**     		XY_GROW_ONLY.  Tells the widget how to change
**     		its display to incorporate the new contents.
*/
void XYSetContents(Widget w, XYCurve *curves, int nCurves, int rescaleMode)
{
    setContents(w, curves, nCurves, rescaleMode, False);
}

/*
** XYUpdateStyles
**
** Update the style information (marker style, line style, color, and marker
** size) for the data currently displayed by the widget, keeping the
** existing data, and ignoring the data fields in the curves structures.
** The number of curves must be the same as the currently displayed data
**
** Parameters
**
**      w		An xy widget
**     curves		An array of curve data structures containing the
**     		new style information, with the same number of 
**     		elements as were passed originally in the nCurves
**     		argument to XYSetContents.
*/
void XYUpdateStyles(Widget w, XYCurve *curves)
{
    int i;
    
    for (i=0; i<((XYWidget)w)->xy.nCurves; i++)
         copyCurveStyle(&curves[i], &((XYWidget)w)->xy.curves[i]);
    if (XtIsRealized(w))
         redisplayContents((XYWidget)w, X_SCREEN, REDRAW_ALL);
}

/*
** XYUpdateData
**
** Update the data displayed by the widget, using the new data in the
** curves structure, ignoring style, color, and name information.
** The number of curves must be the same as the currently displayed data
**
** Parameters
**
**      w		An xy widget
**     curves		An array of curve data structures containing the
**     		new data to display, with the same number of 
**     		elements as were passed originally in the nCurves
**     		argument to XYSetContents.
**     rescaleMode	One of: XY_NO_RESCALE, XY_RESCALE, or
**     		XY_GROW_ONLY.  Tells the widget how to change
**     		its display to incorporate the new contents.
*/
void XYUpdateData(Widget w, XYCurve *curves, int rescaleMode)
{
    setContents(w, curves, ((XYWidget)w)->xy.nCurves, rescaleMode, True);
}

/*
 * JBF - copied from previous version of plot widgets 
 *
** getXYCurves
**
** get the array of points from the widget
**
** Parameters
**
** w An xy widget
**
*/
int
getXYCurves(Widget w, XYCurve **curve)
{
   XYWidget xyW = (XYWidget)w;
 
   *curve = xyW->xy.curves;
   return (xyW->xy.nCurves);
 
}


/*
 * JBF - copied from previous version of plot widgets 
 *
** addPoints
**
** Add more points to the specified curve.
**
** Parameters
**
** w An xy widget
**
** curve_num -- The curve to get the new points
**
** data -- XYPoint data
**
** num_points -- the number of points in the data array
*/
int
addXYPoints(Widget w, int curve_num, XYPoint *data, int num_points)
{
        XYWidget xyW = (XYWidget)w;
        XYPoint *newPoints;
        int i, j;
        int nPoints;
 
        /*
         * make sure a valid curve number was passed in
        */
        if (curve_num >= xyW->xy.nCurves)
                return(-1);
 
        /*
         * make a new array for points, copy the old points into it and then
         * add in the new points
         */
        nPoints = xyW->xy.curves[curve_num].nPoints;
        newPoints = (XYPoint *) malloc(sizeof(XYPoint) * (nPoints + num_points));
        memcpy(newPoints, xyW->xy.curves[curve_num].points,
                                          sizeof(XYPoint) * nPoints);
        for (i=nPoints, j=0; j<num_points; j++, i++)
        {
                newPoints[i].x = data[j].x;
                newPoints[i].y = data[j].y;
        }
        i = sizeof(XYPoint) * (nPoints + num_points);
 
        /*
         * free up the old points data in the curve and set it to the new data
         */
        free((char *)xyW->xy.curves[curve_num].points);
        xyW->xy.curves[curve_num].nPoints = nPoints+num_points;
        xyW->xy.curves[curve_num].points = (XYPoint *) malloc(i);
        memcpy(xyW->xy.curves[curve_num].points, newPoints,  i);
        free((char *)newPoints);
 
    /* redraw the widget with the new data */
    resizeParts(xyW);
    if (XtIsRealized(w))
        redisplayContents(xyW, X_SCREEN, REDRAW_ALL);
        return(0);
}



static void setContents(Widget w, XYCurve *curves, int nCurves, int rescaleMode,
     int dataOnly)
{
    XYWidget xyW = (XYWidget)w;
    int i, redrawArea = REDRAW_NONE;
    int doResize = nCurves != xyW->xy.nCurves && xyW->xy.showLegend;

    /* Free the existing data (but preserve curve structures in dataOnly
       case to save appearance information) */
    if (xyW->xy.nCurves != 0) {
         if (dataOnly) {
             for (i=0; i<xyW->xy.nCurves; i++)
         	freeCurveData(&xyW->xy.curves[i], True);
         } else {
             for (i=0; i<xyW->xy.nCurves; i++)
         	freeCurveData(&xyW->xy.curves[i], False);
             XtFree((char *)xyW->xy.curves);
         }
    }
    
    /* Copy in the user's data */
    if (nCurves != 0) {
         if (!dataOnly) {
             xyW->xy.curves = (XYCurve *)XtMalloc(sizeof(XYCurve) * nCurves);
             xyW->xy.nCurves = nCurves;
         }
         for (i=0; i<nCurves; i++) {
             xyW->xy.curves[i].flatline = 0 ;
             copyCurveData(&curves[i], &xyW->xy.curves[i], dataOnly);
         }
    } else {
         xyW->xy.nCurves = 0;
         xyW->xy.curves = NULL;
    }
    
    /* if the number of curves changed, the legend may have changed size,
       so recalculate the size-dependent parts of the widget */
    if (doResize) {
         resizeParts(xyW);
         redrawArea |= REDRAW_ALL;
    }
    
    /* Scan the data and calculate the new data and view ranges */
    redrawArea |= resetDataAndViewRanges(xyW, rescaleMode);
    
    /* redraw the widget with the new data */
    if (XtIsRealized(w))
         redisplayContents(xyW, X_SCREEN, redrawArea);
}

/*
** Examine the data currently stored in the widget, and reset both the
** maximum viewable data range (minXData, minYData, maxXData, maxYData),
** and the current visible range (minXLim, minYLim, maxXLim, maxYLim),
** according to the specified rescale mode.  Returns which areas of the
** widget have changed and must be redrawn to display the changes.
*/
static int resetDataAndViewRanges(XYWidget w, int rescaleMode)
{    
    double minX, minY, maxX, maxY;
    int redrawArea = REDRAW_NONE;
    
    /* Examine the data to find the min and max values */
    if (w->xy.nCurves == 0) {
         if (rescaleMode == XY_RESCALE) {
             minX = minY = 0.;
             maxX = maxY = 1.;
         } else {
             minX = w->xy.minXData; minY = w->xy.minYData;
             maxX = w->xy.maxXData; maxY = w->xy.maxYData;
         }
    } else
     calcDataRange(w, &minX, &maxX, &minY, &maxY);
    
    /* recalculate plot limits from the new data (full rescale) */
    if (rescaleMode == XY_RESCALE) {
         if (maxX == minX) {
             if ( maxX == 0. ) {
                 maxX += 1.; 
                 minX -= 1.;
             } 
             else if ( maxX > 0. ) {
                 maxX *= 1.01 ; 
                 minX *= 0.99 ;
             } 
             else {
                 maxX *= 0.99 ; 
                 minX *= 1.01 ;
             }
         }
         if (maxY == minY) {
             if ( maxY == 0. ) {
                 maxY += 1.; 
                 minY -= 1.;
             } 
             else if ( maxY > 0. ) {
                 maxY *= 1.01 ; 
                 minY *= 0.99 ;
             } 
             else {
                 maxY *= 0.99 ; 
                 minY *= 1.01 ;
             }
         }
         w->xy.maxXData = maxX; w->xy.maxXLim = maxX;
         w->xy.minXData = minX; w->xy.minXLim = minX;
         w->xy.maxYData = maxY; w->xy.maxYLim = maxY;
         w->xy.minYData = minY; w->xy.minYLim = minY;
         redrawArea |= REDRAW_CONTENTS | REDRAW_H_AXIS | REDRAW_V_AXIS;
    
    /* recalculate data limits but don't change visible range (no rescale) */
    } else if (rescaleMode == XY_NO_RESCALE) {
     redrawArea |= REDRAW_CONTENTS;
     if (maxX > w->xy.maxXData) {
         w->xy.maxXData = maxX;
         redrawArea |= REDRAW_H_AXIS;
     }
     if (minX < w->xy.minXData) {
         w->xy.minXData = minX;
         redrawArea |= REDRAW_H_AXIS;
     }
     if (maxY > w->xy.maxYData) {
         w->xy.maxYData = maxY;
         redrawArea |= REDRAW_V_AXIS;
     }
     if (minY < w->xy.minYData) {
         w->xy.minYData = minY;
         redrawArea |= REDRAW_V_AXIS;
     }
    
    /* recalculate limits for rescale-at-max (normal data-update mode) */
    } else if (rescaleMode == XY_RESCALE_AT_MAX) {
     redrawArea |= REDRAW_CONTENTS;
     minX = dMin(w->xy.minXData, minX);
     maxX = dMax(w->xy.maxXData, maxX);
     minY = dMin(w->xy.minYData, minY);
     maxY = dMax(w->xy.maxYData, maxY);
         if (w->xy.maxXData != maxX || w->xy.minXData != minX)
             redrawArea |= REDRAW_H_AXIS;
         if (w->xy.maxYData != maxY || w->xy.minYData != minY)
             redrawArea |= REDRAW_V_AXIS;
         if (w->xy.maxXData == w->xy.maxXLim)
             w->xy.maxXLim = maxX;
         if (w->xy.minXData == w->xy.minXLim)
             w->xy.minXLim = minX;
         if (w->xy.maxYData == w->xy.maxYLim)
             w->xy.maxYLim = maxY;
         if (w->xy.minYData == w->xy.minYLim)
             w->xy.minYLim = minY;
         w->xy.maxXData = maxX; w->xy.minXData = minX;
        w->xy.maxYData = maxY; w->xy.minYData = minY;

    }
    return redrawArea;
}

/*
** XYSetStrings
**
** Display a set of text strings in the plot area of the widget.  Calling this
** routine removes any strings which were displayed previously and replaces
** them with the ones passed here.  The strings are in the form of XYString
** data structures, which contain position, font, color, and alignment, for
** the string, in addition to the text string itself.  In the XYString
** structure, font can be passed as NULL, and will default to the widget's
** fontList resource.
**
** Parameters
**
**      w		An xy widget
**     strings		Array of XYStrings data structures containing string,
**     		position, font, color, and alignment information
**     nStrings	The number of XYString data structures contained in
**     		strings above
**     
*/
void XYSetStrings(Widget w, XYString *strings, int nStrings)
{
    XYWidget xyW = (XYWidget)w;
    int i;
    
    /* free the old string data */
    for (i=0; i<xyW->xy.nStrings; i++) {
#if XmVERSION < 2
         XmFontListFree(xyW->xy.strings[i].render_font);
#else
        XmRenderTableFree(xyW->xy.strings[i].render_font);
#endif
         XmStringFree(xyW->xy.strings[i].string);
    }
    if (nStrings != 0)
         XtFree((char *)xyW->xy.strings);
    
    /* copy in the new data */
    xyW->xy.nStrings = nStrings;
    if (nStrings == 0)
         xyW->xy.strings = NULL;
    else
         xyW->xy.strings = (XYString *)XtMalloc(sizeof(XYString) * nStrings);
    for (i=0; i<nStrings; i++) {
         xyW->xy.strings[i] = strings[i];
#if XmVERSION < 2
         xyW->xy.strings[i].render_font = XmFontListCopy(strings[i].render_font);
#else
        xyW->xy.strings[i].render_font = XmRenderTableCopy(xyW->xy.render_font,NULL,0);
#endif
         xyW->xy.strings[i].string = XmStringCopy(strings[i].string);
    }
    
    /* update the display */
    if (XtIsRealized(w))
         redisplayContents(xyW, X_SCREEN, REDRAW_CONTENTS);
}

/*
** XYSetVisibleRange, XYGetVisibleRange
**
** Set (Get) the range of data that is visible.  minXLim, minYLim, maxXLim, and
** maxYLim specify the endpoints of the x and y axes.  XYSetVisibleRange,
** unlike the widgets interactive rescaling routines, can zoom out past the
** actual minimum and maximum data points.
*/
void XYSetVisibleRange(Widget w, double minXLim, double minYLim,
     		 double maxXLim, double maxYLim)
{
    XYWidget xyW = (XYWidget)w;
    double minX, minY, maxX, maxY;
    
    /* if log scaling was requested, make sure new range is log scaleable */
    if (xyW->xy.xLogScaling && minXLim <= 0.)
     minXLim = FLT_MIN;
    if (xyW->xy.yLogScaling && minYLim <= 0.)
     minYLim = FLT_MIN;

    /* calculate the actual range of the data */
    calcDataRange(xyW, &minX, &maxX, &minY, &maxY);

    /* allow user to zoom beyond the range of the data */
    xyW->xy.maxXData = dMax(maxXLim, maxX);
    xyW->xy.minXData = dMin(minXLim, minX);
    xyW->xy.maxYData = dMax(maxYLim, maxY);
    xyW->xy.minYData = dMin(minYLim, minY);

    /* Set the range */
    xyW->xy.minXLim = minXLim;
    xyW->xy.maxXLim = maxXLim;
    xyW->xy.minYLim = minYLim;
    xyW->xy.maxYLim = maxYLim;

    /* redraw if the widget is realized */
    if (XtIsRealized(w))
         redisplayContents(xyW, X_SCREEN,
         	REDRAW_V_AXIS | REDRAW_H_AXIS | REDRAW_CONTENTS);
}
void XYGetVisibleRange(Widget w, double *minXLim, double *minYLim,
     		 double *maxXLim, double *maxYLim)
{
    *minXLim = ((XYWidget)w)->xy.minXLim;
    *maxXLim = ((XYWidget)w)->xy.maxXLim;
    *minYLim = ((XYWidget)w)->xy.minYLim;
    *maxYLim = ((XYWidget)w)->xy.maxYLim;
}

/*
** XYZoomOut, XYZoomIn, XYResetZoom
**
** Zoom in and out by ZOOM_FACTOR.  Zoom in is centered on the current
** center of the plot.
*/
void XYZoomOut(Widget w)
{
    XYWidget xyW = (XYWidget)w;
    int xLogScaling = xyW->xy.xLogScaling, yLogScaling = xyW->xy.yLogScaling;
    double xOffset, yOffset, newMaxXLim, newMinXLim, newMaxYLim, newMinYLim;
    double minXLim, maxXLim, minYLim, maxYLim;
    int redrawArea = REDRAW_NONE;
    
    /* if log scaling was requested, express limits in log coordinates */
    minXLim = xLogScaling ? log10(xyW->xy.minXLim) : xyW->xy.minXLim;
    maxXLim = xLogScaling ? log10(xyW->xy.maxXLim) : xyW->xy.maxXLim;
    minYLim = yLogScaling ? log10(xyW->xy.minYLim) : xyW->xy.minYLim;
    maxYLim = yLogScaling ? log10(xyW->xy.maxYLim) : xyW->xy.maxYLim;

    /* Calculate a suitable offset to reverse a zoom in by ZOOM_FACTOR */
    xOffset = (maxXLim - minXLim) * (ZOOM_FACTOR/(1.-ZOOM_FACTOR)) / 2;
    yOffset = (maxYLim - minYLim) * (ZOOM_FACTOR/(1.-ZOOM_FACTOR)) / 2;

    /* widen the plotting limits by the offsets calculated above,
       stopping when the limits reach the limits of the data */
    newMaxXLim = dMin(xyW->xy.maxXData,
             xLogScaling ? pow(10., maxXLim + xOffset) : maxXLim + xOffset);
    newMinXLim = dMax(xyW->xy.minXData,
             xLogScaling ? pow(10., minXLim - xOffset) : minXLim - xOffset);
    newMaxYLim = dMin(xyW->xy.maxYData,
             yLogScaling ? pow(10., maxYLim + yOffset) : maxYLim + yOffset);
    newMinYLim = dMax(xyW->xy.minYData,
             yLogScaling ? pow(10., minYLim - yOffset) : minYLim - yOffset);
    
    /* Tell widget to redraw, and what parts, if limits have changed */
    if (newMaxXLim != maxXLim || newMinXLim != minXLim)
         redrawArea |= REDRAW_H_AXIS | REDRAW_CONTENTS;
    if (newMaxYLim != maxYLim || newMinYLim != minYLim)
         redrawArea |= REDRAW_V_AXIS | REDRAW_CONTENTS;
    
    /* Set the new limits */
    xyW->xy.maxXLim = newMaxXLim;
    xyW->xy.minXLim = newMinXLim;
    xyW->xy.maxYLim = newMaxYLim;
    xyW->xy.minYLim = newMinYLim;

    /* redraw if the widget is realized */
    if (XtIsRealized(w))
         redisplayContents(xyW, X_SCREEN, redrawArea);
}
void XYZoomIn(Widget w)
{
    XYWidget xyW = (XYWidget)w;
    int xLogScaling = xyW->xy.xLogScaling, yLogScaling = xyW->xy.yLogScaling;
    double xOffset, yOffset;
    double minXLim, maxXLim, minYLim, maxYLim;

    /* if log scaling was requested, express limits in log coordinates */
    minXLim = xLogScaling ? log10(xyW->xy.minXLim) : xyW->xy.minXLim;
    maxXLim = xLogScaling ? log10(xyW->xy.maxXLim) : xyW->xy.maxXLim;
    minYLim = yLogScaling ? log10(xyW->xy.minYLim) : xyW->xy.minYLim;
    maxYLim = yLogScaling ? log10(xyW->xy.maxYLim) : xyW->xy.maxYLim;
    
    /* Calculate offsets for limits of displayed data to zoom by ZOOM_FACTOR */
    xOffset = (maxXLim - minXLim) * ZOOM_FACTOR / 2;
    yOffset = (maxYLim - minYLim) * ZOOM_FACTOR / 2;

    /* Narrow the plotting limits by the offsets calculated above */
    maxXLim -= xOffset;
    minXLim += xOffset;
    maxYLim -= yOffset;
    minYLim += yOffset;
    
    /* Set the new limits */
    xyW->xy.maxXLim = xLogScaling ? pow(10.,maxXLim) : maxXLim;
    xyW->xy.minXLim = xLogScaling ? pow(10.,minXLim) : minXLim;
    xyW->xy.maxYLim = yLogScaling ? pow(10.,maxYLim) : maxYLim;
    xyW->xy.minYLim = yLogScaling ? pow(10.,minYLim) : minYLim;
   
    /* redraw if the widget is realized */
    if (XtIsRealized(w))
         redisplayContents(xyW, X_SCREEN,
         	REDRAW_V_AXIS | REDRAW_H_AXIS | REDRAW_CONTENTS);
}
void XYResetZoom(Widget w)
{
    XYWidget xyW = (XYWidget)w;
    double minX, minY, maxX, maxY;

    calcDataRange(xyW, &minX, &maxX, &minY, &maxY);
    xyW->xy.minXLim = xyW->xy.minXData = minX;
    xyW->xy.minYLim = xyW->xy.minYData = minY;
    xyW->xy.maxXLim = xyW->xy.maxXData = maxX;
    xyW->xy.maxYLim = xyW->xy.maxYData = maxY;

    if (XtIsRealized(w))
     redisplayContents(xyW, X_SCREEN,
       	REDRAW_V_AXIS | REDRAW_H_AXIS | REDRAW_CONTENTS);
}

/*
** XYPrintContents
**
** Prints the contents XY widget to a PostScript file.
**
** Parameters
**
**     w		A xy widget
**     psFileName	Name for the PostScript file that will be created
**     
*/
void XYPrintContents(Widget w, char *psFileName)
{
    FILE * ps;
    XYWidget xyW = (XYWidget)w;

    ps = OpenPS(psFileName, xyW->core.width, xyW->core.height);
    if (ps != NULL) {
     redisplayContents(xyW, PS_PRINTER, REDRAW_ALL);
     EndPS();
    }    
}

/*
 * JBF - copied from previous version of plot widgets and modified to
 *       make more generic.
 *
** XYPrintMultiple
**
** Prints the contents of an array of XY widgets to a PostScript file.
**
** Parameters
**
**     w		an array of xy widgets
**     num_plots	number of widgets in array
**     num_rows	number of rows to print on page
**     num_columns	number of columns to print on page
**     page_title	page title for postscript file
**     DestroyOldFile	indicates whether to append to existing file
**                      or to create a new one
**     psFileName	Name for the PostScript file that will be created
**     
*/
void XYPrintMultiple(Widget w[], 
                     int    landscape,
                     int    num_plots,
                     int    num_rows,
                     int    num_columns,
                     char   *page_title, 
                     char   *page_sub_title, 
                     Boolean DestroyOldFile,
                     char   *psFileName)
{
   int      i;
   XYWidget xyW;
   FILE     * ps;
   float    page_x, page_y;
   float    start_page_x, start_page_y;
   float    max_plot_wd_inches, max_plot_ht_inches;
   float    actual_plot_wd_inches, actual_plot_ht_inches;
   float    x_scale_to_page, y_scale_to_page;
   float    scale_factor = 1.0 ;
   int      curr_row, curr_col;
   int      max_wd, max_ht;
   double   title_y_offset ;

   xyW = (XYWidget)w[0];

   ps = OpenPSNoClip(landscape , psFileName, DestroyOldFile, 
                     xyW->core.width, xyW->core.height);

   if (ps == (FILE *)NULL)
      return;


   max_wd = xyW->core.width;
   max_ht = xyW->core.height;
   for (i=1; i<num_plots; i++)
   {
      xyW = (XYWidget)w[i];
      if (max_wd < xyW->core.width)
         max_wd = xyW->core.width;

      if (max_ht < xyW->core.height)
         max_ht = xyW->core.height;
   }

   /*
    * Based on an 8.5 x 11 inch page:
    * Compute the page space needed for each plot widget.  1/2 inch
    * is allowed for page margin in each direction.  Also, 1/2 inch
    * vertically is allowed for the page title.  That leaves a working
    * page area of 8 x 10 inches.
    *
    */
   if ( landscape ) {
      max_plot_wd_inches =  10.0/num_columns;
      max_plot_ht_inches = 8.0/num_rows;
   }
   else {
      max_plot_wd_inches =  8.0/num_columns;
      max_plot_ht_inches = 10.0/num_rows;
   }

   /*
    * Keep the width to height ratio of the printed plot the same as
    * it is on the X interface plot.
    */
   x_scale_to_page = 72.0 * max_plot_wd_inches/max_wd;
   y_scale_to_page = 72.0 * max_plot_ht_inches/max_ht;

   if ( !landscape ) {
      if (x_scale_to_page < y_scale_to_page)
         scale_factor = x_scale_to_page;
      else
         scale_factor = y_scale_to_page;
   }

   actual_plot_wd_inches = scale_factor * ((float)max_wd/72.0);
   actual_plot_ht_inches = scale_factor * ((float)max_ht/72.0);

   /*
    * The scaled plots may not take up the maximum space allowed, so adjust
    * their starting positions so that they will be centered within their
    * allowed space.
    */
   if (max_plot_wd_inches > actual_plot_wd_inches)
      start_page_x = (max_plot_wd_inches - actual_plot_wd_inches)/2.0;
   else
      start_page_x = 0.0;

   if ( landscape ) {
      start_page_x -= 5.0 / num_columns ;
   }

   if (max_plot_ht_inches > actual_plot_ht_inches)
   {
      start_page_y = 10.0 - ((max_plot_ht_inches - actual_plot_ht_inches)/2.0);
      start_page_y -= actual_plot_ht_inches;
   }
   else
      start_page_y = 10.0 - actual_plot_ht_inches;

   if ( landscape ) {
      start_page_y -= ((4.0 / num_rows) + 2.0) ;
   }

   fprintf(ps, "/inch {72 mul} def\n");

   page_x = start_page_x;
   page_y = start_page_y;
   curr_row = 1;
   curr_col = 1;

   for (i=0; i<num_plots; i++)
   {
      xyW = (XYWidget)w[i];

      /*
       * Cause the next plot to print at the desired location on the page.
       */
      fprintf(ps,"gsave\n");

      fprintf(ps,"%f inch %f inch translate\n",page_x, page_y);

      /*
       * Scale the current plot size to 8.5 x 11 page size.
       */
      /*fprintf(ps,"%f %f scale\n", scale_factor,scale_factor);*/
      fprintf(ps,"%f %f scale\n", x_scale_to_page,y_scale_to_page);

      /* Clip to the width and height of the window on the screen */
      fprintf(ps, "0 0 moveto\n");
      fprintf(ps, "%d 0 lineto\n", xyW->core.width);
      fprintf(ps, "%d %d lineto\n", xyW->core.width, xyW->core.height);
      fprintf(ps, "0 %d lineto\n", xyW->core.height);
      fprintf(ps, "closepath clip\n");
      fprintf(ps, "newpath\n");


      redisplayContents(xyW, PS_PRINTER, REDRAW_ALL);

      fprintf(ps,"grestore\n");

      /*
       * Update the current row and column.
       */
      curr_col++;
      if (curr_col > num_columns)
      {
         curr_col = 1;
         curr_row++;
         page_x = start_page_x;
         page_y -= max_plot_ht_inches;
      }
      else
         page_x +=max_plot_wd_inches;

   }

   /*
    * Draw box around page.  A line between each plot will be drawn below
    * in the for loops.  Note that this box and lines are after the 
    * gsave/grestore block containing the tranlate and scale statements for
    * each plot.
    */
   fprintf(ps, "gsave\n");  /* corresponding grestore follows showpage and 
                               is written by EndPS */
   fprintf(ps, "0 0 moveto\n");

   if ( landscape ) {
      fprintf(ps, "10 inch 0 lineto\n");
      fprintf(ps, "10 inch 8.25 inch lineto\n");
      fprintf(ps, "0 8.25 inch lineto\n");
   }
   else {
      fprintf(ps, "8.25 inch 0 lineto\n");
      fprintf(ps, "8.25 inch 10 inch lineto\n");
      fprintf(ps, "0 10 inch lineto\n");
   }

   fprintf(ps, "closepath stroke\n");
   fprintf(ps, "newpath\n");

   for (i=1; i<num_columns; i++)
   {
      page_x = i*max_plot_wd_inches;
      if ( landscape ) {
         fprintf(ps, "%f inch 8 inch moveto\n",page_x);
      }
      else {
         fprintf(ps, "%f inch 10 inch moveto\n",page_x);
      }
      fprintf(ps, "%f inch 0 lineto stroke\n",page_x);
   }

   if ( landscape ) {
      page_y = 8 - max_plot_ht_inches;
   }
   else {
      page_y = 10.0 - max_plot_ht_inches;
   }
   for (i=1; i<num_rows; i++)
   {
      fprintf(ps, "0 %f inch moveto\n",page_y);
      if ( landscape ) {
         fprintf(ps, "10 inch %f inch lineto stroke\n",page_y);
      }
      else {
         fprintf(ps, "8.25 inch %f inch lineto stroke\n",page_y);
      }
      page_y -= max_plot_ht_inches;
   }


   /*
    * Add page_title to postscript file.
    */
   fprintf(ps, "0.00 0.00 0.00 setrgbcolor "
               "/Times-Roman findfont 0012 scalefont setfont\n");


   if ( landscape ) {
           title_y_offset = 8.25 ;
   } else {
           title_y_offset = 10.5 ;
   }
   fprintf(ps,
           "(%s) stringwidth pop 2 div neg 323 add %.2lf inch moveto\n"
           "(%s) show\n" 
           "(%s) stringwidth pop 2 div neg 323 add %.2lf inch 15 neg add moveto\n"
           "(%s) show\n",
           page_title, title_y_offset, page_title,
           page_sub_title, title_y_offset, page_sub_title);

   EndPS();
}

/*
** XYDrawMarker
**
** Draw a marker in an external window (for use with external dialogs for
** setting marker styles).
**
** Parameters
**
**     display		X display
**     drawBuf		X window to draw in
**     gc		X graphics context to use for drawing.  Color, line
**     		thickness and line style will be altered.
**     size		One of: XY_TINY, XY_SMALL, XY_MEDIUM, or XY_LARGE
**     style		One of the valid marker styles as listed in the
**     		description of the curves data structure above.
**     color		Pixel value for the marker
**     x, y		Position in window coordinates of the center of the
**     		marker
*/
void XYDrawMarker(Display *display, Drawable drawBuf, GC gc, int size,
     int style, Pixel color, int x, int y)
{
    XYCurve curve;
    
    curve.markerSize = size;
    curve.markerStyle = style;
    curve.markerPixel = color;
    drawMarker(display, drawBuf, gc, X_SCREEN, &curve, x, y);
}

/*
** XYDrawLine
**
** Draw a line in an external window (for use with external dialogs for
** setting line styles).
**
** Parameters
**
**     display		X display
**     drawBuf		X window to draw in
**     gc		X graphics context to use for drawing.  Color, line
**     		thickness and line style will be altered.
**     style		One of the valid marker styles as listed in the
**     		description of the curves data structure above.
**     color		Pixel value for the marker
**     x1, y1		Position in window coordinates of the start of the line
**     x2, y2		Position in window coordinates of the end of the line
*/
void XYDrawLine(Display *display, Drawable drawBuf, GC gc, int style,
     Pixel color, int x1, int y11, int x2, int y2)
{
    XYCurve curve;
    
    curve.lineStyle = style;
    curve.linePixel = color;
    drawLine(display, drawBuf, gc, X_SCREEN, &curve, x1, y11, x2, y2);
}



/*
** Redisplays the contents part of the widget, (everything but the motif
** shadows and highlights).
*/
static void redisplayContents(XYWidget w, int outDevice, int redrawArea)
{
    Display *display = XtDisplay(w);
    GC gc = w->xy.gc;
    XGCValues gcValues;
    Drawable drawBuf;
    int i, stringWidth, stringX, alignment;
    XmFontList stringFont;
    Pixel lastColor;
    XYString *string;
    transform xform;
#if XmVERSION < 2
    XFontStruct *fs = getFontStruct(w->xy.render_font);
#else
    XFontStruct *fs;
    XmeRenderTableGetDefaultFont(w->xy.render_font,&fs);
#endif
 
    /* Save some energy if the widget isn't visible or no drawing requested */
    if ((outDevice==X_SCREEN && !w->core.visible) || redrawArea == REDRAW_NONE)
        return;

    /* Set destination for drawing commands, offscreen pixmap or window */
    if (w->xy.doubleBuffer)
         drawBuf = w->xy.drawBuffer;
    else
         drawBuf = XtWindow(w);

    /* Clear the drawing buffer or window only in the areas that have
       changed.  The other parts are still redrawn, but the net effect
       is that the unchanged areas do not flicker */
    if (outDevice == X_SCREEN) {
        XSetForeground(display, gc, w->core.background_pixel);
        if (redrawArea == REDRAW_ALL) {
            XFillRectangle(display, drawBuf, gc, w->xy.xMin, w->xy.yMin,
                w->xy.xMax - w->xy.xMin, w->xy.yMax - w->xy.yMin);
        } else {
            if (redrawArea & REDRAW_V_AXIS)
                XFillRectangle(display, drawBuf, gc, w->xy.axisLeft,
                    w->xy.axisTop, w->xy.xOrigin - w->xy.axisLeft,
                    w->xy.axisBottom - w->xy.axisTop);
            if (redrawArea & REDRAW_H_AXIS)
                XFillRectangle(display, drawBuf, gc, w->xy.axisLeft,
                    w->xy.yOrigin + 1, w->xy.axisRight-w->xy.axisLeft,
                    w->xy.axisBottom - w->xy.yOrigin + 1);
            if (redrawArea & REDRAW_CONTENTS)
                XFillRectangle(display, drawBuf, gc, w->xy.xOrigin + 1,
                    w->xy.yEnd, w->xy.xEnd - w->xy.xOrigin,
                    w->xy.yOrigin - w->xy.yEnd);
        }
        /* clear the buffer that hold the value marker.  
           This will tell the routine to start over */
        if ( w->xy.valueBuffer ) {
            XFreePixmap(XtDisplay(w), w->xy.valueBuffer);
            w->xy.valueBuffer = 0 ;
        }
    }
    
    /* Draw the axes */
    gcValues.line_width = 0;
    gcValues.line_style = LineSolid;
    gcValues.foreground = w->primitive.foreground;
    gcValues.font = fs->fid;
    XChangeGC(display, gc, GCLineWidth|GCLineStyle|GCForeground|GCFont, &gcValues);
    if (w->xy.nCurves == 0) {
        /* empty of data, just draw axis lines */
         XSegment segs[2];
         segs[0].x1 = segs[0].x2 = segs[1].x1 = w->xy.xOrigin;
         segs[0].y1 = segs[1].y1 = segs[1].y2 = w->xy.yOrigin;
         segs[1].x2 = w->xy.xEnd; segs[0].y2 = w->xy.yEnd;
     if (outDevice == X_SCREEN)
             XDrawSegments(display, drawBuf, gc, segs, 2);
     else /* PS_PRINTER */
             PSDrawSegments(display, drawBuf, gc, segs, 2);
    } else {

        DrawHorizontalAxis(display, drawBuf, gc, fs, outDevice,
            w->xy.yOrigin, w->xy.yEnd, w->xy.xOrigin, 
            w->xy.xEnd, w->xy.minXData,
            w->xy.maxXData, w->xy.minXLim, w->xy.maxXLim,
            w->xy.xLogScaling, 0, w->xy.showGrid, 
            w->xy.gridColor, w->xy.printGridColor);
        DrawVerticalAxis(display, drawBuf, gc, fs, outDevice,
            w->xy.xOrigin, w->xy.xEnd, w->xy.yEnd, 
            w->xy.yOrigin, w->xy.minYData,
            w->xy.maxYData, w->xy.minYLim, w->xy.maxYLim,
            w->xy.yLogScaling, w->xy.yAxisFormat , 
            w->xy.showGrid, w->xy.gridColor, 
            w->xy.printGridColor);
    }
    

    /* JBF - added plotTitle from old version, but changed XmALIGNMENT_BEGINNING
     * to XmALIGNMENT_CENTER  
     */

   /* Draw the plot title */
    if (w->xy.plotTitle != NULL) {
        if (outDevice == X_SCREEN) {
            XmStringDraw(display, drawBuf, w->xy.render_font, 
                         w->xy.plotTitle, gc,
                         w->xy.xOrigin + Y_LABEL_MARGIN, 
                         w->xy.yMin + TOP_MARGIN,
                         w->xy.xEnd - w->xy.xOrigin, XmALIGNMENT_CENTER,
                         XmSTRING_DIRECTION_L_TO_R, NULL);
        }
        else {
            PSDrawXmString(display, drawBuf, w->xy.render_font, w->xy.plotTitle,
                           gc, w->xy.xOrigin + Y_LABEL_MARGIN, 
                           w->xy.yMin + TOP_MARGIN,
                           w->xy.xEnd - w->xy.xOrigin, XmALIGNMENT_CENTER);
        }
    }
 

    /* Draw the axis labels */
    if (w->xy.xAxisLabel != NULL) {
         if (outDevice == X_SCREEN) {
             XmStringDraw(display, drawBuf, w->xy.render_font, 
                           w->xy.xAxisLabel, gc, 
                           w->xy.xOrigin, 
                           w->xy.axisBottom + X_LABEL_MARGIN,
     	                    w->xy.xEnd - w->xy.xOrigin, 
                           XmALIGNMENT_CENTER,
          	             XmSTRING_DIRECTION_L_TO_R, 
                           NULL);
           }
         else {
             PSDrawXmString(display, drawBuf, w->xy.render_font, 
                            w->xy.xAxisLabel,
     	                     gc, 
                            w->xy.xOrigin, 
                            w->xy.axisBottom + X_LABEL_MARGIN,
     	                     w->xy.xEnd - w->xy.xOrigin, 
                            XmALIGNMENT_CENTER);
       }
    }


    /* JBF - changed y coordinate to reflect addition of plotTitle **/
    if (w->xy.yAxisLabel != NULL) {
         if (outDevice == X_SCREEN) {
             XmStringDraw(display, drawBuf, w->xy.render_font, 
                        w->xy.yAxisLabel, gc,
         	          Y_LABEL_MARGIN, 
                        w->xy.yMin + 2 * TOP_MARGIN + fs->ascent + fs->descent,
     	                 w->xy.xEnd - w->xy.xOrigin, 
                        XmALIGNMENT_BEGINNING,
          	          XmSTRING_DIRECTION_L_TO_R, 
                        NULL);
       }
         else {
             PSDrawXmString(display, drawBuf, w->xy.render_font, 
                            w->xy.yAxisLabel,
         	              gc, 
                            w->xy.xOrigin + Y_LABEL_MARGIN,
                            w->xy.yMin + 2*TOP_MARGIN + 
                                    fs->ascent + fs->descent,
     	                     w->xy.xEnd - w->xy.xOrigin, 
                            XmALIGNMENT_BEGINNING);
       }
    }

    /* Draw the legend */
    if (w->xy.showLegend && redrawArea == REDRAW_ALL)
         drawLegend(w, drawBuf, outDevice);
    
    /* Draw the contents of the plot */
    for (i=0; i<w->xy.nCurves; i++)
         drawCurve(w, drawBuf, outDevice, &w->xy.curves[i]);

    /* Draw the plotted strings */
    lastColor = w->primitive.foreground;
    XSetForeground(display, w->xy.contentsGC, lastColor);
    computeTransform(w, &xform);
    for (i=0, string=w->xy.strings; i<w->xy.nStrings; i++, string++) {
     if (string->color != lastColor) {
         XSetForeground(display, w->xy.contentsGC, string->color);
         lastColor = string->color;
     }
     stringFont = string->render_font == NULL ? 
                    w->xy.render_font : string->render_font ;
     stringWidth = XmStringWidth(stringFont, string->string);
     if (string->alignment == XY_LEFT) {
         stringX = dataToWindowX(&xform, string->x);
         alignment = XmALIGNMENT_BEGINNING;
         } else if (string->alignment == XY_CENTER) {
             stringX = dataToWindowX(&xform, string->x) - stringWidth/2;
             alignment = XmALIGNMENT_CENTER;
         } else { /* XY_RIGHT */
             stringX = dataToWindowX(&xform, string->x) - stringWidth;
             alignment = XmALIGNMENT_END;
         }
         if (outDevice == X_SCREEN)
             XmStringDraw(display, drawBuf, stringFont, string->string,
         	    w->xy.contentsGC, stringX,
         	    (int)dataToWindowY(&xform, string->y), stringWidth,
         	    alignment, XmSTRING_DIRECTION_L_TO_R, NULL);
         else
             PSDrawXmString(display, drawBuf, stringFont, string->string,
         	    w->xy.contentsGC, stringX,
         	    (int)dataToWindowY(&xform, string->y),
         	    stringWidth, alignment);
    }

    /* After zooming, initialize cross hairs */
    w->xy.crosshairFirst = 1 ;
    
    /* For double buffering, now copy offscreen pixmap to screen */
    if (w->xy.doubleBuffer && outDevice == X_SCREEN)
         XCopyArea(display, drawBuf, XtWindow(w), gc, 0, 0,
         	w->core.width, w->core.height, 0, 0);
    
    /* Call the redisplay callback so an application which draws on the xy
       widget can refresh it's graphics */
    if (XtIsRealized((Widget)w) && outDevice == X_SCREEN)
         XtCallCallbacks((Widget)w, XmNredisplayCallback, NULL);
}

/*
** Draw one curve.  Note that drawing is done using the cumbersome method
** of accumulating all of the segments and marker figures for a curve
** in a counted array of X data structures and using the multiple figure
** call, such as XDrawSegments or XDrawRectangles, rather than simply making
** the call to draw the figure on the spot.  This is done because in some
** circumstances there is a 10 to 1 speed difference between these two
** methods.
**
** For the most part the coordinate system
** used for drawing in this widget is the X integer coordinate system
** of the widget's window.  For PostScript output, the coordinates are
** the same, but we sometimes try to acheive higher resolution by using
** floating point numbers to place points within the grid of the 72 dpi
** screen coordinates.  Here, only lines without markers are
** drawn this way.  Improving the print quality of all of the widgets
** is an ongoing process.   
**
** The X coordinate system, being based on 16 bit integers also causes
** trouble with clipping.  If the user zooms out, it is very easy to
** exceed the limits of a short integer.  This widget really needs to
** do its own clipping of lines, but at the moment, it only eliminates those
** lines which are obviously outside of the boundaries of the window.  Lines
** may still have endpoints far outside of the window, and at high zoom
** factors, things happen.
*/
static void drawCurve(XYWidget w, Drawable drawBuf, int outDevice, XYCurve
     *curve)
{
    int hasLines = curve->lineStyle != XY_NO_LINE;
    int hasMarkers = curve->markerStyle != XY_NO_MARK;
    int hasHErrs = curve->horizBars != NULL, hasVErrs = curve->vertBars != NULL;
    Display *display = XtDisplay(w);
    GC gc = w->xy.contentsGC;
    XGCValues gcValues;
    int xMin = w->xy.xOrigin, yMin = w->xy.yEnd;
    int xMax = w->xy.xEnd, yMax = w->xy.yOrigin;
    int markWidth = hasMarkers ? MarkWidths[(int)curve->markerSize] : 0;
    int errWidth = (hasMarkers?markWidth:MED_MARK_SIZE) * ERROR_MARKER_RATIO;
    int xMarkMin = xMin - markWidth/2, yMarkMin = yMin - markWidth/2;
    int xMarkMax = xMax + markWidth/2, yMarkMax = yMax + markWidth/2;
    int xErrMin = xMin - errWidth/2, yErrMin = yMin - errWidth/2;
    int xErrMax = xMax + errWidth/2, yErrMax = yMax + errWidth/2;
    XYErrorBar *hErr, *vErr;
    markStruct markList;
    lineStruct lineList;
    XYPoint *point;
    transform xform;
    XSegment *seg, *segs = NULL ;
    int nSegs;
    double x, y, lastX, lastY, barMin, barMax;
    int i, lastPointDrawn;

    XmString s0 ;
    XtPointer str ;
    double startx = 0.0 ;
#if XmVERSION < 2
    XFontStruct *fs = getFontStruct(w->xy.render_font);
#else
    XFontStruct *fs;
    XmeRenderTableGetDefaultFont(w->xy.render_font,&fs);
#endif
    
    /* Compute constants for data to window coordinate transformation */
    computeTransform(w, &xform);
    
    /* Allocate memory for points, segments and markers */
    if (hasLines)
         startLineList(&lineList, outDevice, curve->nPoints);
    if (hasHErrs || hasVErrs)
         segs = (XSegment *)XtMalloc(sizeof(XSegment) * curve->nPoints *
         	((hasHErrs ? 4 : 0) + (hasVErrs ? 4 : 0)));
    if (hasMarkers)
         startMarkList(&markList, curve->markerStyle, curve->nPoints);

    /* Loop through all of the data converting the data coordinates to
       X coordinates and accumulating line segments and markers to draw. */
    seg = segs;
    nSegs = 0;
    lastPointDrawn = False;
    for (i=0, point=curve->points, hErr=curve->horizBars, vErr=curve->vertBars;
             i<curve->nPoints; i++, point++, hErr++, vErr++) {
         x = dataToWindowX(&xform, point->x);
         y = dataToWindowY(&xform, point->y);
        if (hasLines) {
            if (i!=0 && !((lastX<xMin && x<xMin) || (lastX>xMax && x>xMax) ||
           	    (lastY<yMin && y<yMin) || (lastY>yMax && y>yMax))) {
             if (!lastPointDrawn)
                  addToLineList(&lineList, lastX, lastY, True, hasMarkers);
             if (x<xMin || x>xMax || y<yMin || y>yMax) {
                 /* point was clipped */
                 addToLineList(&lineList, x, y, False, hasMarkers);
                 lastPointDrawn = False;
             } else {
                 addToLineList(&lineList, x, y, False, hasMarkers);
                 lastPointDrawn = True;
                 if ( startx == 0.0 ) {
                     startx = x;
                 }
             }
            }
            lastX = x;
            lastY = y;
        }
        if (hasHErrs && y>yErrMin && y<yErrMax) {
             barMin = dataToWindowX(&xform, hErr->min);
             barMax = dataToWindowX(&xform, hErr->max);
             if (!((barMin<xMin&&barMax<xMin) || (barMin>xMax&&barMax>xMax))) {
         	seg->x1 = barMin; seg->x2 = x-markWidth/2;
         	seg->y1 = seg->y2 = y;
         	seg++; nSegs++;
         	seg->x1 = barMax; seg->x2 = x+markWidth/2;
         	seg->y1 = seg->y2 = y;
         	seg++; nSegs++;
         	seg->x1 = seg->x2 = barMin;
         	seg->y1 = y-errWidth/2; seg->y2 = y+errWidth/2;
         	seg++; nSegs++;
         	seg->x1 = seg->x2 = barMax;
         	seg->y1 = y-errWidth/2; seg->y2 = y+errWidth/2;
         	seg++; nSegs++;
             }
        }
        if (hasVErrs && x>xErrMin && x<xErrMax) {
             barMin = dataToWindowY(&xform, vErr->min);
             barMax = dataToWindowY(&xform, vErr->max);
             if (!((barMin<yMin&&barMax<yMin) || (barMin>yMax&&barMax>yMax))) {
         	seg->x1 = seg->x2 = x; seg->y1 = barMin;
         	seg->y2 = y+markWidth/2;
         	seg++; nSegs++;
          	seg->x1 = seg->x2 = x; seg->y1 = barMax;
          	seg->y2 = y-markWidth/2;
         	seg++; nSegs++;
        	seg->x1 = x-errWidth/2; seg->x2 = x+errWidth/2;
         	seg->y1 = seg->y2 = barMin;
         	seg++; nSegs++;
         	seg->x1 = x-errWidth/2; seg->x2 = x+errWidth/2;
         	seg->y1 = seg->y2 = barMax;
         	seg++; nSegs++;
             }
        }
        if (hasMarkers && x>xMarkMin && x<xMarkMax && y>yMarkMin && y<yMarkMax)
            addToMarkList(&markList, x, y, curve->markerSize);

    }
    
    /* Clipping information can't be read from the GCs by the PS translation
       code, so it has to be set and unset by hand before and after drawing */
    if (outDevice == PS_PRINTER)
         PSSetClipRectangle(xMin, yMin, xMax, yMax);
    
    /* Draw the line segments for the connecting lines */
    if (hasLines)
     drawLineList(display, drawBuf, gc, outDevice, &lineList, curve);
    
    /* write a "flatline" message if the line is truly flat */
    if (curve->flatline != 0) {
        if (outDevice == X_SCREEN) {
           
            char temp[60] ;
            char temp2[60] ;

#if XmVERSION < 2
            str = (char *)w->xy.yAxisFormat ;
#else
            str = XmStringUnparse(w->xy.yAxisFormat, NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
#endif
            sprintf(temp,"Flatline at %s" , (char *)str) ;
            sprintf(temp2, temp , curve->flatlineValue ) ;
            XmStringDraw(display, drawBuf, w->xy.render_font, 
                         s0 = XmStringCreateLocalized(temp2) , gc,
                         (int)startx + 5 ,
                         /*(int)y + fs->ascent ,*/
                         (int)y + 2 ,
                         w->xy.xEnd - w->xy.xOrigin, 
                         XmALIGNMENT_BEGINNING,
                         XmSTRING_DIRECTION_L_TO_R, NULL);

            XtFree(str) ;
            XmStringFree(s0) ;
        }
    }

    /* Draw error bars */
    if (hasHErrs || hasVErrs) {
     gcValues.line_width = 0;
     gcValues.line_style = LineSolid;
     gcValues.foreground = curve->linePixel;
     XChangeGC(display, gc, GCLineWidth|GCLineStyle|GCForeground, &gcValues);
     if (outDevice == X_SCREEN)
             XDrawSegments(display, drawBuf, gc, segs, nSegs);
     else
             PSDrawSegments(display, drawBuf, gc, segs, nSegs);
        XtFree((char *)segs);
    }
    
    /* Draw markers */
    if (hasMarkers)
         drawMarkList(display, drawBuf, gc, outDevice, &markList, curve);
    
    /* Unset temporary clipping on print output */
    if (outDevice == PS_PRINTER)
         PSSetClipRectangle(xMin, yMin, xMax, yMax);
}

/*
** Allocate memory and initialize a lineList data structure to begin
** accumulating groups of lines to draw.  Lines are accumulated either
** in XPoint form, for the screen, or FloatPoint form for higher resolution
** on a PostScript printer.  Used in conjunction with addToLineList and
** drawLineList.  drawLineList deallocates the memory allocated here
*/
static void startLineList(lineStruct *lineList, int outDevice, int maxSegs)
{
    if (outDevice == X_SCREEN)
         lineList->fillPtr = lineList->points =
         	(XPoint *)XtMalloc(sizeof(XPoint) * maxSegs * 2);
    else
         lineList->fFillPtr = lineList->floatPoints =
         	(FloatPoint *)XtMalloc(sizeof(FloatPoint) * maxSegs * 2);
    lineList->lineCounts = (int *)XtMalloc(sizeof(int) * maxSegs);
    lineList->curLineCount = lineList->lineCounts;
    *lineList->curLineCount = 0;
    lineList->outDevice = outDevice;
}

/*
** Add a new point to an accumulating line to be drawn later.  See
** startLineList and drawLineList for more information.  restart tells the
** function to begin a new run of connected segments (i.e. don't draw
** a line from the last point to this one).  If round is true, rounds
** x and y to the nearest integer value.  This is used in the case of
** printer output to reduce the resolution, since markers are not yet drawn
** at the higher resolution, and the lines don't look good if they don't
** join the markers (esp. small ones) exactly.
*/
static void addToLineList(lineStruct *lineList, float x, float y, int restart,
     int round)
{
    if (restart && *lineList->curLineCount != 0) {
     lineList->curLineCount++;
     *lineList->curLineCount = 0;
    }
    if (lineList->outDevice == X_SCREEN) {
     lineList->fillPtr->x = x;
     lineList->fillPtr->y = y;
     lineList->fillPtr++;
    } else {
     lineList->fFillPtr->x = round ? (int)x : x;
     lineList->fFillPtr->y = round ? (int)y : y;
     lineList->fFillPtr++;
    }
    (*lineList->curLineCount)++;
}

/*
** drawLineList draws the lines accumulated in lineList (see startLineList
** and addToLineList).  The argument "curve" supplies style information.
** Uses repeated calls to either XDrawLines or PSFloatDrawLines.  Also
** deallocates memory attached to lineList, allocated by startLineList.
*/
static void drawLineList(Display *display, Drawable drawBuf, GC gc,
     int outDevice, lineStruct *lineList, XYCurve *curve)
{
    XGCValues gcValues;
    XPoint *pt;
    FloatPoint *fpt;
    int nLineCounts = (lineList->curLineCount - lineList->lineCounts) + 1;
    int i, *count, style = curve->lineStyle;
    
    /* set up the graphics context.  the convention for GCs in this widget
       is that line style, width, and color are not preserved */
    gcValues.line_width = style == XY_THICK_LINE ? 2 :
             (style == XY_X_THICK_LINE ? 3 : 0);
    gcValues.foreground = curve->linePixel;
    gcValues.line_style = (style==XY_PLAIN_LINE || style==XY_THICK_LINE ||
             style==XY_X_THICK_LINE) ? LineSolid : LineOnOffDash;
    XChangeGC(display, gc, GCLineWidth|GCLineStyle|GCForeground, &gcValues);
    XSetDashes(display, gc, 0, DashLists[style], strlen(DashLists[style]));

    if (outDevice == X_SCREEN) {
        pt = lineList->points;
        for (i=0, count=lineList->lineCounts; i<nLineCounts; i++, count++) {
         XDrawLines(display, drawBuf, gc, pt, *count, 0);
             pt += *count;
        }
        XtFree((char *)lineList->points);
    } else { /* outDevice == PS_PRINTER */
        fpt = lineList->floatPoints;
        for (i=0, count=lineList->lineCounts; i<nLineCounts; i++, count++) {
         PSFloatDrawDashedLines(display, drawBuf, gc, fpt, *count,
     	    DashLists[style], 0);
             fpt += *count;
        }
        XtFree((char *)lineList->floatPoints);
    }
    XtFree((char *)lineList->lineCounts);
}

/*
** allocate storage for segments, rectangles or arcs for drawing markers
*/
static void startMarkList(markStruct *markList, int markerStyle, int maxLength)
{
    int size;
    
    markList->style = markerStyle;
    switch (markerStyle) {
      case XY_NO_MARK:
           size = 0;
           break;
      case XY_SQUARE_MARK: case XY_SOLID_SQUARE_MARK: case XY_THICK_SQUARE_MARK:
            size = sizeof(XRectangle);
        break;
      case XY_CIRCLE_MARK: case XY_SOLID_CIRCLE_MARK: case XY_THICK_CIRCLE_MARK:
           size = sizeof(XArc);
        break;
      case XY_STAR_MARK:
           size = sizeof(XSegment) * 4;
        break;
      case XY_X_MARK:
           size = sizeof(XSegment) * 2;
        break;
      case XY_TRIANGLE_MARK:
           size = sizeof(XSegment) * 3;
        break;
    }
    markList->list = (void *)XtMalloc(size * maxLength);
    markList->fillPtr = markList->list;
    markList->nFigures = 0;
}

/*
** Add the appropriate figure(s) to markList to eventually draw a marker at
** the location (x,y).  The list must have been allocated with startMarkList,
** and should be drawn and disposed of with drawMarkList.
*/
static void addToMarkList(markStruct *markList, int x, int y, int size)
{
    int markWidth = MarkWidths[size];

    switch (markList->style) {
      case XY_NO_MARK:
           break;
      case XY_SQUARE_MARK: case XY_SOLID_SQUARE_MARK: case XY_THICK_SQUARE_MARK:
           ((XRectangle *)markList->fillPtr)->x = x - markWidth/2;
           ((XRectangle *)markList->fillPtr)->y = y - markWidth/2;
           ((XRectangle *)markList->fillPtr)->width = markWidth;
           ((XRectangle *)markList->fillPtr)->height = markWidth;
           markList->fillPtr += sizeof(XRectangle);
           markList->nFigures++;
           break;
      case XY_CIRCLE_MARK: case XY_SOLID_CIRCLE_MARK: case XY_THICK_CIRCLE_MARK:
           ((XArc *)markList->fillPtr)->x = x - markWidth/2;
           ((XArc *)markList->fillPtr)->y = y - markWidth/2;
           ((XArc *)markList->fillPtr)->width = markWidth;
           ((XArc *)markList->fillPtr)->height = markWidth;
           ((XArc *)markList->fillPtr)->angle1 = 0;
           ((XArc *)markList->fillPtr)->angle2 = 360 * 64;
           markList->fillPtr += sizeof(XArc);
           markList->nFigures++;
        break;
      case XY_STAR_MARK:
          ((XSegment *)markList->fillPtr)->x1 = x;
           ((XSegment *)markList->fillPtr)->y1 = y - markWidth/2;
           ((XSegment *)markList->fillPtr)->x2 = x;
           ((XSegment *)markList->fillPtr)->y2 = y + markWidth/2;
           markList->fillPtr += sizeof(XSegment);
           ((XSegment *)markList->fillPtr)->x1 = x - markWidth/2;
           ((XSegment *)markList->fillPtr)->y1 = y;
           ((XSegment *)markList->fillPtr)->x2 = x + markWidth/2;
           ((XSegment *)markList->fillPtr)->y2 = y;
           markList->fillPtr += sizeof(XSegment);
           markList->nFigures += 2;
           /* no break, uses XY_X_MARK */
      case XY_X_MARK:
           ((XSegment *)markList->fillPtr)->x1 = x - markWidth/2;
           ((XSegment *)markList->fillPtr)->y1 = y - markWidth/2;
           ((XSegment *)markList->fillPtr)->x2 = x + markWidth/2;
           ((XSegment *)markList->fillPtr)->y2 = y + markWidth/2;
           markList->fillPtr += sizeof(XSegment);
           ((XSegment *)markList->fillPtr)->x1 = x + markWidth/2;
           ((XSegment *)markList->fillPtr)->y1 = y - markWidth/2;
           ((XSegment *)markList->fillPtr)->x2 = x - markWidth/2;
           ((XSegment *)markList->fillPtr)->y2 = y + markWidth/2;
           markList->fillPtr += sizeof(XSegment);
           markList->nFigures += 2;
           break;
      case XY_TRIANGLE_MARK:
           ((XSegment *)markList->fillPtr)->x1 = x;
           ((XSegment *)markList->fillPtr)->y1 = y - markWidth/2;
           ((XSegment *)markList->fillPtr)->x2 = x + markWidth/2;
           ((XSegment *)markList->fillPtr)->y2 = y + markWidth/2;
           markList->fillPtr += sizeof(XSegment);
           ((XSegment *)markList->fillPtr)->x1 = x + markWidth/2;
           ((XSegment *)markList->fillPtr)->y1 = y + markWidth/2;
           ((XSegment *)markList->fillPtr)->x2 = x - markWidth/2;
           ((XSegment *)markList->fillPtr)->y2 = y + markWidth/2;
           markList->fillPtr += sizeof(XSegment);
           ((XSegment *)markList->fillPtr)->x1 = x - markWidth/2;
           ((XSegment *)markList->fillPtr)->y1 = y + markWidth/2;
           ((XSegment *)markList->fillPtr)->x2 = x;
           ((XSegment *)markList->fillPtr)->y2 = y - markWidth/2;
           markList->fillPtr += sizeof(XSegment);
           markList->nFigures += 3;
        break;
    }
}

/*
** Draw the markers accumulated with addToMarkList and free the list
*/
static void drawMarkList(Display *display, Drawable drawBuf, GC gc,
     int outDevice, markStruct *markList, XYCurve *curve)
{
    XGCValues gcValues;
    int size = curve->markerSize, style = curve->markerStyle;
    
    /* set up the graphics context.  the convention for GCs in this widget
       is that line style, width, and color are not preserved */
    if (style==XY_THICK_SQUARE_MARK || style==XY_THICK_CIRCLE_MARK)
         gcValues.line_width = size==XY_SMALL ? 1 :
                 (size==XY_MEDIUM || size==XY_LARGE ? 2 : 0);
    else
         gcValues.line_width = 0;
    gcValues.line_style = LineSolid;
    gcValues.foreground = curve->markerPixel;
    XChangeGC(display, gc, GCLineWidth|GCLineStyle|GCForeground, &gcValues);

    if (outDevice == X_SCREEN) {
     switch (style) {
       case XY_NO_MARK:
               break;
       case XY_SQUARE_MARK: case XY_THICK_SQUARE_MARK:
            XDrawRectangles(display, drawBuf, gc, (XRectangle *)markList->list,
                 markList->nFigures);
            break;
        case XY_SOLID_SQUARE_MARK:
            XFillRectangles(display, drawBuf, gc, (XRectangle *)markList->list,
                 markList->nFigures);
            break;
       case XY_CIRCLE_MARK: case XY_THICK_CIRCLE_MARK:
            XDrawArcs(display, drawBuf, gc, (XArc *)markList->list,
                 markList->nFigures);
            break;
       case XY_SOLID_CIRCLE_MARK:
            XFillArcs(display, drawBuf, gc, (XArc *)markList->list,
                 markList->nFigures);
            break;
       case XY_STAR_MARK: case XY_X_MARK: case XY_TRIANGLE_MARK:
            XDrawSegments(display, drawBuf, gc, (XSegment *)markList->list,
                 markList->nFigures);
            break;
     }
    } else { /* PS_PRINTER */
     switch (style) {
       case XY_NO_MARK:
               break;
       case XY_SQUARE_MARK: case XY_THICK_SQUARE_MARK:
            PSDrawRectangles(display, drawBuf, gc, (XRectangle *)markList->list,
                 markList->nFigures);
            break;
        case XY_SOLID_SQUARE_MARK:
            PSFillRectangles(display, drawBuf, gc, (XRectangle *)markList->list,
                 markList->nFigures);
            break;
       case XY_CIRCLE_MARK: case XY_THICK_CIRCLE_MARK:
            PSDrawArcs(display, drawBuf, gc, (XArc *)markList->list,
                 markList->nFigures);
            break;
       case XY_SOLID_CIRCLE_MARK:
            PSFillArcs(display, drawBuf, gc, (XArc *)markList->list,
                 markList->nFigures);
            break;
       case XY_STAR_MARK: case XY_X_MARK: case XY_TRIANGLE_MARK:
            PSDrawSegments(display, drawBuf, gc, (XSegment *)markList->list,
                 markList->nFigures);
            break;
     }
    }
    XtFree((char *)markList->list);
}

/*
** Find the best layout for a legend for the current set of named curves that
** the widget contains.  Returns the number of rows, height, column width, and
** left margin for the optimal layout.
*/
static void layoutLegend(XYWidget w, int *nRows, int *legendHeight,
     int *columnWidth, int *leftMargin)
{
    int xOrigin = w->xy.xOrigin, xEnd = w->xy.xEnd;
    int n, row, rows, colHeight, maxColHeight, width=0, nCols=0; /* JBF - inited*/
    XYCurve *curve;
        
    /* If the legend is hidden */
    if (!w->xy.showLegend) {
         *nRows = 0;
         *legendHeight = 0;
         *leftMargin = 0;
         *columnWidth = 0;
         return;
    }
    
    /* Find how many rows and columns will fit all of the curve names,
       and the width of that layout for centering later */
    for (rows=1; rows<=w->xy.nCurves; rows++)
         if (tryLegendLayout(w, rows, &width, &nCols))
             break;
    *nRows = rows;
    
    /* Find the height of the legend */
    maxColHeight = 0;
    row = 1;
    colHeight = 0;
    for (curve=w->xy.curves, n=0; n<w->xy.nCurves; curve++, n++) {
         colHeight += XmStringHeight(w->xy.render_font, curve->name);
         if (row >= rows) {
             maxColHeight = colHeight > maxColHeight ? colHeight : maxColHeight;
             row = 1;
             colHeight = 0;
         } else
             row++;
    }
    *legendHeight = maxColHeight;
    
    /* determine a pleasant vertical spacing (column spacing and margins),
       with the goal of centering the legend under the horizontal axis line
       with the widest possible column spacing, but ignoring centering to
       pack the legend in as few rows as possible. */
    if (width > xEnd - xOrigin +
             2 * ((w->xy.xMax - LEGEND_RIGHT_MARGIN) - w->xy.xEnd)) {
         /* can't center, just move left margin */
         *columnWidth = LEGEND_COLUMN_MARGIN;
         *leftMargin = w->xy.xMax - LEGEND_RIGHT_MARGIN - width;
         if (*leftMargin < w->xy.xMin + LEGEND_LEFT_MARGIN)
             *leftMargin = w->xy.xMin + LEGEND_LEFT_MARGIN;
    } else if (width > xEnd - xOrigin || nCols <= 1) {
         /* can center, but can't adjust column spacing */
         *columnWidth = LEGEND_COLUMN_MARGIN;
         *leftMargin = xOrigin + ((xEnd - xOrigin) - width)/2;
    } else {
         /* can center and widen column spacing */
         *columnWidth = ((xEnd - xOrigin) - (width -
         	(LEGEND_COLUMN_MARGIN*(nCols-1)))) / (nCols-1);
         *leftMargin = xOrigin;
    }
}

/*
** Draw the plot legend at the bottom of the widget
*/
static void drawLegend(XYWidget w, Drawable drawBuf, int outDevice)
{
    Display *display = XtDisplay(w);
    GC gc = w->xy.gc;
    int markerWidth, lineWidth, style;
    int n, top_, left, row, col, maxNameWidth, nameWidth, nameHeight;
    int sampleY, sampleX1, sampleX2, nameX;
    XYCurve *curve;
    
    top_ = w->xy.legendTop;
    left = w->xy.legendLeftMargin;
    row = 1;
    col = 1;
    maxNameWidth = 0;
    style = legendColumnStyle(w->xy.curves, w->xy.nCurves, 1, w->xy.legendRows);
    for (curve=w->xy.curves, n=0; n<w->xy.nCurves; curve++, n++) {
         markerWidth = (style==NO_MARKERS ? 0 : LARGE_MARK_SIZE);
         lineWidth = (style==NO_LINES ? 0 : LEGEND_LINE_LEN);
         nameHeight = XmStringHeight(w->xy.render_font, curve->name);
         nameWidth = XmStringWidth(w->xy.render_font, curve->name);
         sampleY = top_ + nameHeight/2;
         sampleX1 = left + markerWidth/2;
         sampleX2 = sampleX1 + lineWidth;
         nameX = left + LEGEND_NAME_SPACING + markerWidth + lineWidth;
         if (curve->lineStyle != XY_NO_LINE)
             drawMarker(display, drawBuf, gc, outDevice, curve,
             	    sampleX1, sampleY);
         drawMarker(display, drawBuf, gc, outDevice, curve, sampleX2, sampleY);
         drawLine(display, drawBuf, gc, outDevice, curve,
             	sampleX1, sampleY, sampleX2, sampleY);
         XSetForeground(display, gc, w->primitive.foreground);
         if (outDevice == X_SCREEN)
             XmStringDraw(display, drawBuf, w->xy.render_font, curve->name, gc,
         	    nameX, top_, nameWidth, XmALIGNMENT_BEGINNING,
          	    XmSTRING_DIRECTION_L_TO_R, NULL);
         else
             PSDrawXmString(display, drawBuf, w->xy.render_font, curve->name, gc,
         	    nameX, top_, nameWidth, XmALIGNMENT_BEGINNING);
     maxNameWidth = nameWidth > maxNameWidth ? nameWidth : maxNameWidth;
     if (row >= w->xy.legendRows) {
         top_ = w->xy.legendTop;
             left = nameX + maxNameWidth + w->xy.legendColumnSpacing;
             row = 1;
             col++;
             maxNameWidth = 0;
             style = legendColumnStyle(w->xy.curves, w->xy.nCurves, col,
             	    w->xy.legendRows);
     } else {
         top_ += nameHeight;
         row++;
     }
    }
}

/*
** Find out if a layout using a given number of rows is possible, and if
** so, the minimum width and number of columns it would contain.
*/
static int tryLegendLayout(XYWidget w, int nRows, int *width, int *nColumns)
{
    int curve, colStart, colWidth, row, fieldWidth, nCols, style;
    int maxWidth = (w->xy.xMax - LEGEND_RIGHT_MARGIN) -
             (w->xy.xMin + LEGEND_LEFT_MARGIN);
    
    curve = 0;
    colStart = 0;
    nCols = 0;
    while (curve < w->xy.nCurves) {
        nCols++;
        style = legendColumnStyle(w->xy.curves, w->xy.nCurves, nCols, nRows);
        colWidth = 0;
        for (row=1; row<=nRows; row++) {
            fieldWidth = (style==NO_LINES ? 0 : LEGEND_LINE_LEN) +
                     (style==NO_MARKERS ? 0 : LARGE_MARK_SIZE) +
                     LEGEND_NAME_SPACING + (w->xy.curves[curve].name == NULL ?
                     0 : XmStringWidth(w->xy.render_font, w->xy.curves[curve].name));
            colWidth = fieldWidth > colWidth ? fieldWidth : colWidth;
            if (++curve >= w->xy.nCurves)
                 break;
        }
        if (colStart + colWidth > maxWidth  &&  nRows < w->xy.nCurves)
            return False;
        colStart += colWidth + LEGEND_COLUMN_MARGIN;
    }
    *width = colStart - LEGEND_COLUMN_MARGIN;
    *nColumns = nCols;
    return True;
}

/*
** Given a column number and the number of rows in the legend, calculates
** how the column should appear: NO_LINES, NO_MARKERS, or FULL_LEGEND
*/
static int legendColumnStyle(XYCurve *curves, int nCurves, int colNum,
     int nRows)
{
    int i, hasMarkers = False, hasLines = False;
    
/** JBF **
    for (i=(colNum-1)*nRows; i<nCurves, i<colNum*nRows; i++) {}
***/
    for (i=(colNum-1)*nRows; ((i<nCurves) && (i<colNum*nRows)); i++) {
/** end JBF **/
         if (curves[i].markerStyle != XY_NO_MARK)
             hasMarkers = True;
         if (curves[i].lineStyle != XY_NO_LINE)
             hasLines = True;
    }
    if (hasMarkers && hasLines) return FULL_LEGEND;
    else if (hasLines) return NO_MARKERS;
    else return NO_LINES;
}

/*
** Draw a single marker of style defined by curve, centered on (x, y)
*/
static void drawMarker(Display *display, Drawable drawBuf, GC gc, int outDevice,
     XYCurve *curve, int x, int y)
{
    markStruct markList;

    if (curve->markerStyle == XY_NO_MARK)
         return;
    startMarkList(&markList, curve->markerStyle, 1);
    addToMarkList(&markList, x, y, curve->markerSize);
    drawMarkList(display, drawBuf, gc, outDevice, &markList, curve);
}

/*
** Draw a single line of style defined by curve from (x1, y1) to (x2, y2)
*/
static void drawLine(Display *display, Drawable drawBuf, GC gc, int outDevice,
     XYCurve *curve, int x1, int y11, int x2, int y2)
{
    lineStruct lineList;
    
    if (curve->lineStyle == XY_NO_LINE)
         return;
    startLineList(&lineList, outDevice, 2);
    addToLineList(&lineList, x1, y11, True, False);
    addToLineList(&lineList, x2, y2, False, False);
    drawLineList(display, drawBuf, gc, outDevice, &lineList, curve);
}

/*
** For smoother animation, it is possible to allocate an offscreen pixmap
** and draw to that rather than directly into the window.  Unfortunately,
** it is too slow on many machines, so we have to give the user the choice
** whether to use it or not.  This routine reallocates the offscreen
** pixmap buffer when the window is resized, and when the widget is created
*/
static void updateBufferAllocation(XYWidget w)
{ 
    if (w->xy.drawBuffer)
         XFreePixmap(XtDisplay(w), w->xy.drawBuffer);
    if (w->xy.doubleBuffer) {
         w->xy.drawBuffer = XCreatePixmap(XtDisplay(w),
     	DefaultRootWindow(XtDisplay(w)), w->core.width, w->core.height,
          	DefaultDepthOfScreen(XtScreen(w)));   
    } else {
         w->xy.drawBuffer = 0;
    }

}

#if XmVERSION >= 2

/*
** In Motif 2.0 and later, XmFontList is an obsolete data type.
** Use XmRenderTable instead. 
*/
static void DefaultFont(
   Widget        w,
   int           offset __attribute__ ((unused)),
   XrmValue     *value)
{
   static XmRenderTable   rt;

   rt = XmeGetDefaultRenderTable(w,XmTEXT_RENDER_TABLE);
   value->addr = (XtPointer) &rt;
   value->size = sizeof(rt);

} /* end DefaultFont */

#else /* Motif 1.x */
/*
** Get the XFontStruct that corresponds to the default (first) font in
** a Motif font list.  Since Motif stores this, it saves us from storing
** it or querying it from the X server.
*/
#if ((XmVERSION >= 1) && (XmREVISION >= 2))
/*
 * JBF - updated for motif 1.2 
 *       XmFontListGetNextFont is obsolete in Motif 1.2
 */
 
/* Motif 1.2 or newer */
static XFontStruct *getFontStruct(XmFontList font)
{
    XFontStruct *fs;
    XmFontContext context;
    XmFontListEntry     entry;
    XmFontType          type_return;
    XtPointer           font_info;
    int                 num_fs;
    XFontStruct         **font_struct_list_return;
    char                **font_name_list_return;
 
    XmFontListInitFontContext(&context, font);
    entry = XmFontListNextEntry(context);
    if (entry != (XmFontListEntry) NULL)
    {
       font_info = XmFontListEntryGetFont(entry,&type_return);
       if (type_return == XmFONT_IS_FONT)
          fs = (XFontStruct *)font_info;
       else /* type_return == XmFONT_IS_FONTSET */
       {
          num_fs = XFontsOfFontSet((XFontSet)font_info,
                                   &font_struct_list_return,
                                   &font_name_list_return);
          if (num_fs > 0)
             fs = font_struct_list_return[0];
          else
             fs = (XFontStruct *)NULL;
       }
    }
    else
       fs = (XFontStruct *)NULL;
 
    XmFontListFreeFontContext(context);
    return fs;
}
 
#else
/* Motif 1.1 */

static XFontStruct *getFontStruct(XmFontList font)
{
    XFontStruct *fs;
    XmFontContext context;
    XmStringCharSet charset;

    XmFontListInitFontContext(&context, font);
    XmFontListGetNextFont(context, &charset, &fs);
    XmFontListFreeFontContext(context);
    XtFree(charset);
    return fs;
}
#endif /* for Motif 1.2 or greater */
#endif /* for Motif 2 of greater */


/*
** Calculate the actual extent of the data in the widget.  As a side-effect,
** may reset xLogScaling or yLogScaling to false if data can not be displayed
** in log scaling mode.
*/
static void calcDataRange(XYWidget w, double *xMin, double *xMax, double *yMin,
     double *yMax)
{
    XYCurve *curve;
    double minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
    double cMinX, cMaxX, cMinY, cMaxY, logMin, logMax;
    int n;
 
    /* combine the ranges from all of the curves */
    for (n=0, curve=w->xy.curves; n<w->xy.nCurves; n++, curve++) {
     oneCurveDataRange(w, curve, &cMinX, &cMaxX, &cMinY, &cMaxY);
     minX = dMin(minX, cMinX);
     maxX = dMax(maxX, cMaxX);
     minY = dMin(minY, cMinY);
     maxY = dMax(maxY, cMaxY);
     if ( cMinY == cMaxY ) {
             curve->flatline = 1 ;
             curve->flatlineValue = cMinY ;
     }
    }
    
    /* Check that log scaling is possible with current data and current
       settings of xLogScaling and yLogScaling.  If it is not possible, 
       resets x or yLogScaling to False and takes no further action.  */
    if (w->xy.xLogScaling && minX <= 0.)
     w->xy.xLogScaling = False;
    if (w->xy.yLogScaling && minY <= 0.)
     w->xy.yLogScaling = False;
    
    /* expand the range by plot margin percentage (used mostly to make
       room for markers and error bar tops) */
    if (w->xy.xLogScaling) {
         logMin = log10(minX);
         logMax = log10(maxX);
         minX = pow(10., logMin - (logMax - logMin) * w->xy.marginPercent / 100);
         maxX = pow(10., logMax + (logMax - logMin) * w->xy.marginPercent / 100);
    } else {
     minX -= (maxX - minX) * w->xy.marginPercent / 100;
     maxX += (maxX - minX) * w->xy.marginPercent / 100;
    }
    if (w->xy.yLogScaling) {
         logMin = log10(minY);
         logMax = log10(maxY);
         minY = pow(10., logMin - (logMax - logMin) * w->xy.marginPercent / 100);
         maxY = pow(10., logMax + (logMax - logMin) * w->xy.marginPercent / 100);
    } else {
     minY -= (maxY - minY) * w->xy.marginPercent / 100;
     maxY += (maxY - minY) * w->xy.marginPercent / 100;
    }
    
    *xMin = minX; *yMin = minY; *xMax = maxX; *yMax = maxY;
}

/*
** Find the rectangular area covered by a curve, including error bars and
** decorations (markers and error bar ends)
*/
static void oneCurveDataRange(XYWidget w __attribute__ ((unused)), XYCurve *curve, double *xMin,
     double *xMax, double *yMin, double *yMax)
{
    XYPoint *point;
    XYErrorBar *vert, *hor;
    double minX = FLT_MAX, minY = FLT_MAX, maxX = -FLT_MAX, maxY = -FLT_MAX;
    int i;

    /* Loop through all of the data, recording minimum and maximum values */
    if (curve->vertBars != NULL && curve->horizBars != NULL) {
     for (i=0, point=curve->points, vert=curve->vertBars,
         	hor=curve->horizBars; i<curve->nPoints;
         	i++, point++, vert++, hor++) {
             if (point->x > maxX) maxX = point->x;
             if (hor->max > maxX) maxX = hor->max;
             if (point->y > maxY) maxY = point->y;
             if (vert->max > maxY) maxY = vert->max;
             if (point->x < minX) minX = point->x;
             if (hor->min < minX) minX = hor->min;
             if (point->y < minY) minY = point->y;
             if (vert->min < minY) minY = vert->min;
     }
    } else if (curve->vertBars != NULL) {
     for (i=0, point=curve->points, vert=curve->vertBars;
         	i<curve->nPoints; i++, point++, vert++) {
             if (point->x > maxX) maxX = point->x;
             if (point->y > maxY) maxY = point->y;
             if (vert->max > maxY) maxY = vert->max;
             if (point->x < minX) minX = point->x;
             if (point->y < minY) minY = point->y;
             if (vert->min < minY) minY = vert->min;
     }
    } else if (curve->horizBars != NULL) {
     for (i=0, point=curve->points, hor=curve->horizBars;
         	i<curve->nPoints; i++, point++, hor++) {
             if (point->x > maxX) maxX = point->x;
             if (hor->max > maxX) maxX = hor->max;
             if (point->y > maxY) maxY = point->y;
             if (point->x < minX) minX = point->x;
             if (hor->min < minX) minX = hor->min;
             if (point->y < minY) minY = point->y;
     }
    } else {
     for (i=0, point=curve->points; i<curve->nPoints; i++, point++) {
             if (point->x > maxX) maxX = point->x;
             if (point->y > maxY) maxY = point->y;
             if (point->x < minX) minX = point->x;
             if (point->y < minY) minY = point->y;
     }
    }
    
    *xMin = minX; *yMin = minY; *xMax = maxX; *yMax = maxY;
}    

/* Compute constants for data to window coordinate transformation */
static void computeTransform(XYWidget w, transform *xform)
{    
    /* If log scaling was requested, express limits in log coordinates */
    if (w->xy.xLogScaling) {
         xform->minXData = log10(w->xy.minXData);
         xform->minXLim = log10(w->xy.minXLim);
         xform->maxXLim = log10(w->xy.maxXLim);
    } else {
         xform->minXData = w->xy.minXData;
         xform->minXLim = w->xy.minXLim;
         xform->maxXLim = w->xy.maxXLim;
    }
    if (w->xy.yLogScaling) {
         xform->minYData = log10(w->xy.minYData);
         xform->minYLim = log10(w->xy.minYLim);
         xform->maxYLim = log10(w->xy.maxYLim);
    } else {
         xform->minYData = w->xy.minYData;
         xform->minYLim = w->xy.minYLim;
         xform->maxYLim = w->xy.maxYLim;
    }
    xform->xScale = (w->xy.xEnd - w->xy.xOrigin) /
             (xform->maxXLim - xform->minXLim);
    xform->yScale = (w->xy.yOrigin - w->xy.yEnd) /
             (xform->maxYLim - xform->minYLim);
    xform->minXPix = w->xy.xOrigin -
             (xform->minXLim - xform->minXData) * xform->xScale;
    xform->maxYPix = w->xy.yOrigin +
             (xform->minYLim - xform->minYData) * xform->yScale;
    xform->xLogScaling = w->xy.xLogScaling;
    xform->yLogScaling = w->xy.yLogScaling;
}

/* Convert X and Y values from data coordinates to window coordinates.
   Requires valid transformation data from computeTransform above */
static double dataToWindowX(transform *xform, double value)
{
    if (xform->xLogScaling)
     return xform->minXPix + (log10(value)-xform->minXData) * xform->xScale;
    else
     return xform->minXPix + (value - xform->minXData) * xform->xScale;
}
static double dataToWindowY(transform *xform, double value)
{
    if (xform->yLogScaling)
     return xform->maxYPix - (log10(value)-xform->minYData) * xform->yScale;
    else
     return xform->maxYPix - (value - xform->minYData) * xform->yScale;
}

/* Copies data from one curve data structure to another */
static void copyCurveData(XYCurve *fromCurve, XYCurve *toCurve, int dataOnly)
{
        int pointDataSize, errDataSize;
    
        if (!dataOnly) {
             copyCurveStyle(fromCurve, toCurve);
             toCurve->nPoints = fromCurve->nPoints;
        }

        pointDataSize = sizeof(XYPoint) * fromCurve->nPoints;
        toCurve->points = (XYPoint *)XtMalloc(pointDataSize);

        memcpy(toCurve->points, fromCurve->points, pointDataSize);
        errDataSize = sizeof(XYErrorBar) * fromCurve->nPoints;

        if (fromCurve->horizBars != NULL) {
             toCurve->horizBars = (XYErrorBar *)XtMalloc(errDataSize);
             memcpy(toCurve->horizBars, fromCurve->horizBars, errDataSize);
        } else {
            toCurve->horizBars = NULL;
        }

        if (fromCurve->vertBars != NULL) {
                toCurve->vertBars = (XYErrorBar *)XtMalloc(errDataSize);
                memcpy(toCurve->vertBars, fromCurve->vertBars, errDataSize);
        } else {
                toCurve->vertBars = NULL;
        }

}

/* Copies name and style information from one curve data structure to another */
static void copyCurveStyle(XYCurve *fromCurve, XYCurve *toCurve)
{
    toCurve->curve_id = fromCurve->curve_id == NULL ?
             NULL : XmStringCopy(fromCurve->curve_id);

    toCurve->name = fromCurve->name == NULL ?
             NULL : XmStringCopy(fromCurve->name);

    toCurve->markerStyle = fromCurve->markerStyle;
    toCurve->markerSize = fromCurve->markerSize;
    toCurve->lineStyle = fromCurve->lineStyle;
    toCurve->markerPixel = fromCurve->markerPixel;
    toCurve->linePixel = fromCurve->linePixel;
}

/* Free the data WITHIN a curve data structure (not the structure itself) */
static void freeCurveData(XYCurve *curve, int dataOnly)
{
    if (!dataOnly && curve->curve_id != NULL)
         XmStringFree(curve->curve_id);
    if (!dataOnly && curve->name != NULL)
         XmStringFree(curve->name);
    if (curve->points != NULL)
         XtFree((char *)curve->points);
    if (curve->horizBars != NULL)
         XtFree((char *)curve->horizBars);
    if (curve->vertBars != NULL)
         XtFree((char *)curve->vertBars);
}

/* minimum and maximum of two doubles */
static double dMin(double d1, double d2)
{
    if (d2 < d1)
         return d2;
    return d1;
}
static double dMax(double d1, double d2)
{
    if (d2 > d1)
         return d2;
    return d1;
}


/*
** Find the distance from a point to a curve 
** 
** x/y_on_curve are the closest points on
** the curve from x0, y0
**
** This was fun to write 
** Keith's Addition
** 7-11-2003 
*/
double dist_point_to_curve( 
     XYCurve* curve,                              /* input */
     double x0, double y00,                        /* inputs */
     double* x_on_curve, double* y_on_curve ) {   /* outputs */
     
     int ii ;
     double m ;
     double b1, b2 ;
     double dist, dist2, min_dist ;
     double x1, y11, x2, y2 ;
     double x2_x1, y2_y1 ;
     double xi, yi ;
     double xi_x0, yi_y0  ;
     double x1_x0, y1_y0, x2_x0, y2_y0 ;

     int onSegment ;
     int inXRange ;
     int inYRange ;

     min_dist = 1.0e30 ;
     for ( ii = 0 ; ii < curve->nPoints-1 ; ii++ ) {

           /* Get shorthand names */
           x1 = curve->points[ii].x ; 
           y11 = curve->points[ii].y ;
           x2 = curve->points[ii+1].x ; 
           y2 = curve->points[ii+1].y ; ;
           x2_x1 = x2 - x1 ;
           y2_y1 = y2 - y11 ;

           /* Find distance from (x0,y0) to line defined by (x1,y1) (x2,y2) */
           /* Find what point is closest from (x0,y0) to line segment 
            * (i.e. solve two equations where lines intersect)
            *
            * You could solve distance right now with the following: 
            * It is just a novelty, I don't use it.
            * d = fabs( m*(x0 - x1) - y0 + y11 ) / sqrt( m*m + 1 ) ;  
            */
           if ( y2_y1 == 0.0 ) {
                xi = x0 ;
                yi = y11 ;
           } else if ( x2_x1 == 0.0 ) {
                xi = x1 ; 
                yi = y00 ;
           } else { 
                m = ( y2_y1 ) / ( x2_x1 ) ;
                b1 = y00 + x0/m ; 
                b2 = y11 - m*x1 ; 
                xi = (b1 - b2)*m/(m*m + 1) ;
                yi = m*xi + b2 ;
           }

           /* Is (xi,yi) on (x1,y11)(x2,y2) segment? */
           onSegment = 0 ;
           inXRange = 0 ;
           inYRange = 0 ;
           if ( x1 < x2 ) {
                if ( x1 <= xi && xi <= x2 ) {
                     inXRange = 1 ;
                } 
           } else {
                if ( x2 <= xi && xi <= x1 ) {
                     inXRange = 1 ;
                }
           }
           if ( y11 < y2 ) {
                if ( y11 <= yi && yi <= y2 ) {
                     inYRange = 1 ;
                } 
           } else {
                if ( y2 <= yi && yi <= y11 ) {
                     inYRange = 1 ;
                }
           }
           if ( inXRange && inYRange ) {
                onSegment = 1 ;
           }
           if ( onSegment ) {
                xi_x0 = xi - x0 ;
                yi_y0 = yi - y00 ;
                dist = xi_x0*xi_x0 + yi_y0*yi_y0  ;
                if ( dist < min_dist ) {
                     min_dist = dist ;
                     *x_on_curve = xi ;
                     *y_on_curve = yi ;
                }
           } else {
                /* Intersection point not on segment, so
                 * closest point will be one of the segment's endpoints
                 */
                x1_x0 = x1 - x0 ;
                y1_y0 = y11 - y00 ;
                x2_x0 = x2 - x0 ;
                y2_y0 = y2 - y00 ;
                dist  = x1_x0*x1_x0 + y1_y0*y1_y0 ;
                dist2 = x2_x0*x2_x0 + y2_y0*y2_y0 ;
                if ( dist < min_dist ) {
                     min_dist = dist ;
                     *x_on_curve = xi ;
                     *y_on_curve = yi ;
                }
                if ( dist2 < min_dist ) {
                     min_dist = dist2 ;
                     *x_on_curve = xi ;
                     *y_on_curve = yi ;
                }
           }
     }

     return ( sqrt(min_dist) ) ;
}

/*
** This routine will draw a line with connecting text 
** for a given point on the widget 
** The given point is designated in data coords
** Keith's Addition
** 7-11-2003 
*/
void label_pointer( XYWidget w, double x_data, double y_data, 
                    char* label_text ) {

    int x_rect, y_rect ;
    int w_rect, h_rect ; 

    int x_win, y_win ;
    transform xform;
    XGCValues gcValues;
    XmString x_string ;

    /* For color */
    XColor xWindowsColorScreen ;
    XColor xWindowsColorExact ;
    Screen * screen ;
    Colormap colormap ;
    Pixel yellow_pixel ;
    Pixel black_pixel ;

    /* For font size */
    XFontStruct *fs;

    /* For XDrawLine */
    Display *display = XtDisplay(w);
    Drawable drawBuf ;
    GC gc = w->xy.gc;
    
    if (w->xy.doubleBuffer) {
         drawBuf = w->xy.drawBuffer;
    } else {
         drawBuf = XtWindow(w);
    } 

    /* Compute constants for data to window coordinate transformation */
    computeTransform(w, &xform);

    x_win = dataToWindowX(&xform, x_data);
    y_win = dataToWindowY(&xform, y_data);

    /* Get font info for font size */
#if XmVERSION < 2
    fs = getFontStruct(w->xy.render_font) ;
#else
    XmeRenderTableGetDefaultFont(w->xy.render_font,&fs);
#endif

    /* Get yellow and black */
    screen   = XtScreen(w) ;
    colormap = DefaultColormapOfScreen(screen);
    XAllocNamedColor(display, colormap, "yellow",
                     &xWindowsColorScreen,
                     &xWindowsColorExact ) ;
    yellow_pixel = xWindowsColorScreen.pixel ;
    XAllocNamedColor(display, colormap, "black",
                     &xWindowsColorScreen,
                     &xWindowsColorExact ) ;
    black_pixel = xWindowsColorScreen.pixel ;

    if ( w->xy.axisRight > x_win + 180 ) {

         /* Draw pointer label to right of line */

         /* Draw a little arrow to line */
         XDrawLine( display, drawBuf, gc, x_win, y_win, 
                                          x_win, y_win - 6 ) ;
         XDrawLine( display, drawBuf, gc, x_win, y_win, 
                                          x_win + 6 , y_win ) ;
         XDrawLine( display, drawBuf, gc, x_win, y_win, 
                                          x_win + 30 , y_win - 30 ) ;
         XDrawLine( display, drawBuf, gc, x_win + 30 , y_win - 30 , 
                                          x_win + 60 , y_win - 30 ) ;  

         /* Get pixel location for little box to put label in */
         x_rect = x_win + 60 ;
         y_rect = y_win - 30 - fs->ascent/2 - 3 ; 

    } else {
         /* If box will extend outside of viewing area on right,
          * then make box on left hand side of line
          */

         /* Draw a little arrow to line */
         XDrawLine( display, drawBuf, gc, x_win, y_win, 
                                          x_win, y_win - 6 ) ;
         XDrawLine( display, drawBuf, gc, x_win, y_win, 
                                          x_win - 6 , y_win ) ;
         XDrawLine( display, drawBuf, gc, x_win, y_win, 
                                          x_win - 30 , y_win - 30 ) ;
         XDrawLine( display, drawBuf, gc, x_win - 30 , y_win - 30 , 
                                          x_win - 60 , y_win - 30 ) ;  

         /* Get pixel location for little box to put label in */
         x_rect = x_win - 90 ;
         y_rect = y_win - 30 - fs->ascent/2 - 3 ; 
    }
    w_rect = fs->max_bounds.width*strlen(label_text) + 6 ; 
    h_rect = fs->ascent + fs->descent + 6 ; 

    /* Draw a little box to put label in */
    XDrawRectangle( display, drawBuf, gc, 
                    x_rect, y_rect, w_rect, h_rect ) ; 
    gcValues.foreground = (unsigned long) yellow_pixel ;
    XChangeGC(display, gc, GCForeground, &gcValues);
    XFillRectangle( display, drawBuf, gc, 
                    x_rect+1, y_rect+1, w_rect-1, h_rect-1 ) ; 
    
    /* Draw the text of the label */
    gcValues.foreground = (unsigned long) black_pixel ;
    XChangeGC(display, gc, GCForeground, &gcValues);
    x_string = XmStringCreateLocalized( label_text ) ; 
    XmStringDraw(display, drawBuf, w->xy.render_font, x_string, gc,
                 x_rect + 3, y_rect + h_rect/2 - fs->ascent/2 - 1,
                 strlen(label_text) + 6, XmALIGNMENT_BEGINNING,
                 XmSTRING_DIRECTION_L_TO_R, NULL);
    XmStringFree( x_string ) ;

    /* For double buffering, now copy offscreen pixmap to screen */
    if (w->xy.doubleBuffer) {
         XCopyArea(display, drawBuf, XtWindow(w), w->xy.gc, 0, 0,
         	w->core.width, w->core.height, 0, 0);
    }
    
    /* Call the redisplay callback so an application which draws on the xy
       widget can refresh it's graphics */
    if (XtIsRealized((Widget)w))
         XtCallCallbacks((Widget)w, XmNredisplayCallback, NULL);
}


/* 
 ** Keith snagged this off of the internet 
 ** You have to have:
 **    #include <X11/Intrinsic.h>
 **    #include <X11/IntrinsicP.h>
 **    #include <X11/CoreP.h>
 */
void GetWidgetPopupList(Widget w, WidgetList *wl, int *num) {

      struct _WidgetRec *wr = (struct _WidgetRec *) w;

      *wl = wr->core.popup_list;
      *num = wr->core.num_popups;
}

/*
 ** This pops up a dialog box for the user to enter formulas to
 ** transform a curves x-y vectors
 ** Keith's Addition
 ** 7-11-2003 
 */
void manipulateCurve(XYWidget w, XEvent *event, char* args __attribute__ ((unused)), int n_args __attribute__ ((unused))) {

     double xOnCurve, yOnCurve ;
     char title[128] ;

     int ii ;
     int nPopupChildren ;
     WidgetList popupChildren ;

     static XYManipulateCurveCBData xym ;
     Widget dialog ;
     Widget pane_main ;
     Widget form_control ;
     Widget form_action ;
     Widget label_x ;
     Widget label_y ;
     static Widget text_x ;
     static Widget text_y ;
     Widget button_ok ;
     Widget button_close ;
     Widget button_apply ;
     Dimension h ;
     XmString label_string ;

     /* Get position and curve selected */
     w->xy.selectedCurve = curveClosestToMouseClick( w, event, 
                                                    &xOnCurve, &yOnCurve ) ;
     /* Set dialog box title */
     sprintf(title, "Manipulate Curve #%d",  w->xy.selectedCurve ) ;

     /* If there is already a dialog box up, reset title and return */
     GetWidgetPopupList(XtParent(w), &popupChildren, &nPopupChildren) ;
     for ( ii = 0 ; ii < nPopupChildren ; ii++ ) {
          if ( ! strcmp( XtName(popupChildren[ii]), "xy_formula_dialog" ) )  {
               XtVaSetValues( popupChildren[ii],
                              XmNtitle,  title,
                              NULL ) ;
               selectCurve(w, event, NULL, 0 ) ;
               xym.w = w ;  /* Switch current widget to one just clicked */
               return ;
          }
     }

     dialog = XtVaCreatePopupShell( "xy_formula_dialog", 
                                    xmDialogShellWidgetClass, XtParent(w),
                                    XmNtitle,                 title,
                                    XmNdeleteResponse,        XmDESTROY, 
                                    NULL ) ;

     pane_main = XtVaCreateWidget( "pane_main",
                                xmPanedWindowWidgetClass, dialog,
                                XmNsashWidth,             1,
                                XmNsashHeight,            1,
                                XmNheight,                175, 
                                XmNwidth,                 400, 
                                NULL ) ;

     /* 
      * The "control" area of the dialog 
      */
     form_control = XtVaCreateWidget( "form_control",
                                xmFormWidgetClass,    pane_main,
                                NULL ) ; 

     label_x = XtVaCreateManagedWidget( "x = ",
                              xmLabelGadgetClass,   form_control,
                              XmNtopOffset,         15,
                              XmNtopAttachment,     XmATTACH_POSITION,
                              XmNbottomAttachment,  XmATTACH_NONE,
                              XmNleftAttachment,    XmATTACH_FORM,
                              NULL ) ;

     text_x = XtVaCreateManagedWidget( "text_x_formula",
                              xmTextWidgetClass,    form_control,
                              XmNvalue,             "x",
                              XmNtopOffset,         10,
                              XmNtopAttachment,     XmATTACH_FORM,
                              XmNbottomAttachment,  XmATTACH_NONE,
                              XmNleftAttachment,    XmATTACH_WIDGET,
                              XmNleftWidget,        label_x,   
                              XmNrightAttachment,   XmATTACH_FORM,   
                              NULL ) ;

     label_y = XtVaCreateManagedWidget( "y = ",
                              xmLabelGadgetClass,   form_control,
                              XmNtopOffset,         25,
                              XmNtopAttachment,     XmATTACH_WIDGET,
                              XmNtopWidget,         text_x,
                              XmNbottomAttachment,  XmATTACH_NONE,
                              XmNleftAttachment,    XmATTACH_FORM,
                              NULL ) ;

     text_y = XtVaCreateManagedWidget( "text_y_formula",
                              xmTextWidgetClass,    form_control,
                              XmNvalue,             "y",
                              XmNtopOffset,         20,
                              XmNtopAttachment,     XmATTACH_WIDGET,
                              XmNtopWidget,         text_x,
                              XmNbottomAttachment,  XmATTACH_NONE,
                              XmNleftAttachment,    XmATTACH_WIDGET,
                              XmNleftWidget,        label_y,   
                              XmNrightAttachment,   XmATTACH_FORM,   
                              NULL ) ;

     /* 
      * The "action" area of the dialog 
      */
     xym.x_formula = text_x ; 
     xym.y_formula = text_y ; 
     xym.w         = w ;

     form_action = XtVaCreateWidget( "form_action",
                                xmFormWidgetClass,    pane_main,
                                XmNfractionBase,      7,
                                NULL ) ; 

     /* OK */
     label_string = XmStringCreateLocalized("OK") ;
     button_ok = XtVaCreateManagedWidget( "button_ok",
                                     xmPushButtonGadgetClass, form_action,
                                     XmNdefaultButtonShadowThickness, 1,
                                     XmNlabelString,          label_string,
                                     XmNtopAttachment,        XmATTACH_FORM,
                                     XmNbottomAttachment,     XmATTACH_FORM,
                                     XmNleftAttachment,       XmATTACH_POSITION,
                                     XmNleftPosition,         1,
                                     XmNrightAttachment,      XmATTACH_POSITION,
                                     XmNrightPosition,        2,
                                     NULL ) ;
     XmStringFree(label_string) ;
     XtAddCallback( button_ok,
                    XmNactivateCallback,
                    manipulateCurveCB,
                    &xym ) ;

     /* Apply */
     label_string = XmStringCreateLocalized("Apply") ;
     button_apply = XtVaCreateManagedWidget( "button_apply",
                                     xmPushButtonGadgetClass, form_action,
                                     XmNdefaultButtonShadowThickness, 1,
                                     XmNlabelString,          label_string,
                                     XmNtopAttachment,        XmATTACH_FORM,
                                     XmNbottomAttachment,     XmATTACH_FORM,
                                     XmNleftAttachment,       XmATTACH_POSITION,
                                     XmNleftPosition,         3,
                                     XmNrightAttachment,      XmATTACH_POSITION,
                                     XmNrightPosition,        4,
                                     NULL ) ;
     XmStringFree(label_string) ;
     XtAddCallback( button_apply,
                    XmNactivateCallback,
                    manipulateCurveCB,
                    &xym ) ;

     /* Close */
     label_string = XmStringCreateLocalized("Close") ;
     button_close = XtVaCreateManagedWidget( "button_close",
                                     xmPushButtonGadgetClass, form_action,
                                     XmNdefaultButtonShadowThickness, 1,
                                     XmNlabelString,          label_string,
                                     XmNtopAttachment,        XmATTACH_FORM,
                                     XmNbottomAttachment,     XmATTACH_FORM,
                                     XmNleftAttachment,       XmATTACH_POSITION,
                                     XmNleftPosition,         5,
                                     XmNrightAttachment,      XmATTACH_POSITION,
                                     XmNrightPosition,        6,
                                     NULL ) ;
     XmStringFree(label_string) ;
     XtAddCallback( button_close,
                    XmNactivateCallback,
                    manipulateCurveCB,
                    &xym ) ;

     /* Action area sizing */
     XtVaGetValues( button_ok, XmNheight, &h, NULL ) ;
     XtVaSetValues( form_action,
                    XmNpaneMaximum,  h,
                    XmNpaneMinimum,  h,
                    NULL ) ;

     XtManageChild(pane_main) ;
     XtManageChild(form_action) ;
     XtManageChild(form_control) ;
     XtManageChild(dialog) ;
     XtPopup( dialog, XtGrabNone ) ;

     selectCurve(w, event, NULL, 0 ) ;
}

/*
 ** Callback when buttons pressed on
 ** manipulateCurve() dialog box 
 ** Keith's Addition
 ** 7-11-2003 
 */
void manipulateCurveCB ( Widget widget, 
                         XtPointer client_data, 
                         XtPointer call_data  __attribute__ ((unused))) {
     int ii ;
     int ret ;
     XYManipulateCurveCBData* xym ;
     XYCurve* curve ;
     char *xEquation ;
     char *yEquation ;

     double val ;
                                                                                
     if ( !strcmp( XtName(widget), "button_ok") ||
          !strcmp( XtName(widget), "button_apply") ) {
                                                                                
          xym = (XYManipulateCurveCBData*) client_data ;
                                                                                
          xEquation = XmTextGetString (xym->x_formula) ;
          yEquation = XmTextGetString (xym->y_formula) ;
          curve = &(xym->w->xy.curves[xym->w->xy.selectedCurve]) ;

          /* Make sure that there are no problems with curve (i.e.
           * syntax errors, divide by zero, square root neg num .... )
           * We are not manipulating curve, we are just error checking
           */
          for ( ii = 0 ; ii < curve->nPoints ; ii++ ) {
               ret = equationparse(xEquation,
                                   curve->points[ii].x,
                                   &val) ;
               if ( ret ) {
                    errorEQParseDialog( XtParent(XtParent(widget)), ret ) ;
                    return ; 
               }

               ret =  equationparse(yEquation,
                                    curve->points[ii].y,
                                    &val ) ;
               if ( ret ) {
                    errorEQParseDialog( XtParent(XtParent(widget)), ret ) ;
                    return ; 
               }
          }
    
          /* Do actual curve manipulation */
          for ( ii = 0 ; ii < curve->nPoints ; ii++ ) {
               ret = equationparse(xEquation,
                                   curve->points[ii].x,
                                   &curve->points[ii].x) ;
               ret =  equationparse(yEquation,
                                    curve->points[ii].y,
                                    &curve->points[ii].y ) ;
          }
                                                                                
          resetDataAndViewRanges(xym->w, XY_RESCALE);
          redisplayContents(xym->w, X_SCREEN, REDRAW_ALL);
     }
                                                                                
     if ( !strcmp( XtName(widget), "button_ok") ||
          !strcmp( XtName(widget), "button_close") ) {
                                                                                
          /* Close out the dialog box */
          while ( ! XmIsDialogShell( widget ) ) {
               widget = XtParent( widget ) ;
          }
          XtDestroyWidget(widget);
     }
}

static void motion(XYWidget w, XEvent *event, char *args, int n_args) {

    /* 
       if we are using crosshairs, call that first.  valueDisplay
       will be called inside the crossHairs routine
     */
    if (  w->xy.enableCrosshairs ) {
        crossHairs( w , event , args , n_args ) ;
    }
    else {
        if (  w->xy.valueDisplay ) {
            valueDisplay( w , event , args , n_args ) ;
        }
    }

}

static void valueDisplay(XYWidget w, XEvent *event, char *args __attribute__ ((unused)), int n_args __attribute__ ((unused))) {

    int ii , jj ;
    XYCurve * curve ;
    XYPoint * point ;
    double dist ;
    double min_dist ;
    double min_x , min_y ;
    char format[60] ;
    char value[60] ;
    transform xform;
    int x_point, y_point ;
    int x_win , y_win ;
    Display *display ;
    Drawable drawBuf ;
    XmString s0 ;
    XtPointer str ;
    GC gc ;
    Pixel markColor ;
    XFontStruct *fs;
    int y_rect ;
    int w_rect , h_rect ; 
    int xMin = w->xy.xOrigin, yMin = w->xy.yEnd;
    int xMax = w->xy.xEnd, yMax = w->xy.yOrigin;
    int clear_width ;

    if (w->xy.doubleBuffer) {
         drawBuf = w->xy.drawBuffer;
    } else {
         drawBuf = XtWindow(w);
    } 

    /* For XDrawLine */
    display = XtDisplay(w);
    
    gc = w->xy.gc;

    /* Get font info for font size */
#if XmVERSION < 2
    fs = getFontStruct(w->xy.render_font) ;
#else
    XmeRenderTableGetDefaultFont(w->xy.render_font,&fs);
#endif

    computeTransform(w, &xform);
    min_dist = 1e200;
    /* find the closest point to the cursor */
    for ( ii = 0 ; ii < w->xy.nCurves ; ii++ ) {
        curve = &(w->xy.curves[ii]) ;
        for ( jj = 0 ; jj < curve->nPoints ; jj++ ) {
            point = &(curve->points[jj]) ;

            x_point = dataToWindowX(&xform, point->x);
            y_point = dataToWindowY(&xform, point->y);

            /* make sure the point is on the screen */
            if (x_point >= xMin && x_point <= xMax && 
                y_point >= yMin && y_point <= yMax) {

                /* get magnitude of distance, no need for sqrt */
                dist = (x_point - event->xbutton.x)*(x_point - event->xbutton.x) + 
                 (y_point - event->xbutton.y)*(y_point - event->xbutton.y) ;

                if ( dist < min_dist ) {
                    min_dist = dist ;
                    min_x = point->x ;
                    min_y = point->y ;
                    x_win = x_point ;
                    y_win = y_point ;
                    markColor = curve->linePixel;
                }
            }
        }
    }

    if ( w->xy.valueBuffer == 0 ) {
       /* 
          valueBuffer will contain the graph before a mark is placed on it
          we use it to copy back the original picture when we clear a mark
        */
        w->xy.valueBuffer = XCreatePixmap(XtDisplay(w),
         DefaultRootWindow(XtDisplay(w)), w->core.width, w->core.height,
         DefaultDepthOfScreen(XtScreen(w)));   

        /* copy the clean drawbuf to valueBuffer for saving */
        XCopyArea(display, drawBuf , w->xy.valueBuffer, gc, 0 , 0 ,
         w->core.width , w->core.height , 0 , 0 );
    } 
    else {
        /* clear the last mark by copying the clean valueBuffer back to drawBuf */
        XCopyArea(display, w->xy.valueBuffer, drawBuf , gc, w->xy.valueX - LARGE_MARK_SIZE , w->xy.valueY - LARGE_MARK_SIZE ,
         LARGE_MARK_SIZE * 2, LARGE_MARK_SIZE * 2, w->xy.valueX - LARGE_MARK_SIZE , w->xy.valueY - LARGE_MARK_SIZE );
        /* copy to the screen too */
        XCopyArea(display, w->xy.valueBuffer, XtWindow(w) , gc, w->xy.valueX - LARGE_MARK_SIZE , w->xy.valueY - LARGE_MARK_SIZE ,
         LARGE_MARK_SIZE * 2, LARGE_MARK_SIZE * 2, w->xy.valueX - LARGE_MARK_SIZE , w->xy.valueY - LARGE_MARK_SIZE );
    }

    /* draw a mark on the closest point */
    XYDrawMarker(display, drawBuf, gc, XY_LARGE,
     XY_SOLID_CIRCLE_MARK , markColor, x_win, y_win) ;

#if XmVERSION < 2
    str = (char *)w->xy.yAxisFormat ;
#else
    str = (XtPointer)XmStringUnparse(w->xy.yAxisFormat, NULL, 0, XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
#endif
    sprintf(format,"%s , %s" , (char *)str, (char *)str ) ;
    sprintf(value, format , min_x , min_y ) ;

    /* calculate maximum width we need to clear 2*format max width + 3*width of char + pad(6) */
    clear_width = VAxisWidth(fs , w->xy.yAxisFormat) * 2 + fs->max_bounds.width * 3 + 6 ;
    h_rect = fs->ascent + fs->descent + 6 ; 
    y_rect = w->xy.yMin + 2 * TOP_MARGIN + fs->ascent + fs->descent ; 

    XSetForeground(display, gc, w->core.background_pixel);
    XFillRectangle( display, drawBuf, gc, 
                    w->xy.xEnd - clear_width , y_rect, clear_width, h_rect ) ; 

    w_rect = fs->max_bounds.width*strlen(value) + 6 ; 

    /* draw the value string */
    XSetForeground(display, gc, w->primitive.foreground);
    s0 = XmStringCreateLocalized( value ) ; 
    XmStringDraw(display, drawBuf, w->xy.render_font,
                s0, gc,
                w->xy.xEnd - w_rect ,
                w->xy.yMin + 2 * TOP_MARGIN + fs->ascent + fs->descent,
                w->xy.xEnd - w->xy.xOrigin,
                XmALIGNMENT_BEGINNING,
                XmSTRING_DIRECTION_L_TO_R,
                NULL);

    XmStringFree( s0 ) ;
    XtFree(str) ;

    /* For double buffering, now copy offscreen pixmap to screen , copy only the changed area */
    if (w->xy.doubleBuffer) {
        XCopyArea(display, drawBuf , XtWindow(w), gc, w->xy.xEnd - clear_width , y_rect ,
         clear_width , h_rect , w->xy.xEnd - clear_width , y_rect );
        XCopyArea(display, drawBuf , XtWindow(w), gc, x_win - LARGE_MARK_SIZE , y_win - LARGE_MARK_SIZE ,
         LARGE_MARK_SIZE * 2 , LARGE_MARK_SIZE * 2 , x_win - LARGE_MARK_SIZE , y_win - LARGE_MARK_SIZE );
    }
     
    /* save the coordinates of the last mark */
    w->xy.valueX = x_win ;
    w->xy.valueY = y_win ;

    return ;
}


static void crossHairs(XYWidget w, XEvent *event, char *args, int n_args) {

     double x, y ;
     int x_win, y_win ;
     Display *display ;
     Drawable drawBuf ;
     transform xform;
 
     /* If user prefers -not- to use crosshair, just return */
     if ( ! w->xy.enableCrosshairs ) {
          return ;
     }

     /* For XDrawLine */
     display = XtDisplay(w);
    
     if (w->xy.doubleBuffer) {
          drawBuf = w->xy.drawBuffer;
     } else {
          drawBuf = XtWindow(w);
     } 

     /* Compute constants for data to window coordinate transformation */
     computeTransform(w, &xform);

      /* Get mouse position in pixel coords */
      getXYMousePosition( w, event, &x, &y ) ;
      x_win = dataToWindowX(&xform, x);
      y_win = dataToWindowY(&xform, y);

      /* Don't draw any cross hair outside the axes */
      if ( x_win < w->xy.xOrigin || x_win > w->xy.xEnd  || 
           y_win > w->xy.yOrigin || y_win < w->xy.yEnd   ) {
           return ;
      }
 
      /* Draw cross hair and data labels */
      if ( w->xy.crosshairFirst ) {

           /* call valueDisplay while the graph is clear */
           if (  w->xy.valueDisplay ) {
                valueDisplay( w , event , args , n_args ) ;
           }

           sprintf(w->xy.crosshairVertText, "%.6lf", x) ;
           sprintf(w->xy.crosshairHoriText, "%.6lf", y) ;

           drawCrosshairs( display, drawBuf, w, x_win, y_win ) ;

           /* Save off this first state */
           w->xy.lastCrosshairX = x_win ;
           w->xy.lastCrosshairY = y_win ;
           w->xy.crosshairFirst = 0 ;

     } else {

           /* Clear last line and labels 
            * (the magic is in the GCXxor func in zoomGC -> a^b^b = a) 
            */
           drawCrosshairs( display, drawBuf, w,
                           w->xy.lastCrosshairX, w->xy.lastCrosshairY ) ;
            
           /* call valueDisplay while the graph is clear */
           if (  w->xy.valueDisplay ) {
                valueDisplay( w , event , args , n_args ) ;
           }

           /* Draw the crosshairs with current data */
           sprintf(w->xy.crosshairVertText, "%.6lf", x) ;
           sprintf(w->xy.crosshairHoriText, "%.6lf", y) ;
           drawCrosshairs( display, drawBuf, w, x_win, y_win ) ;
           
           /* Save off the last coords to clear hairs on next call */
           w->xy.lastCrosshairX = x_win ;
           w->xy.lastCrosshairY = y_win ;
     }

     /* For double buffering, now copy offscreen pixmap to screen */
     if (w->xy.doubleBuffer) {
          XCopyArea(display, drawBuf, XtWindow(w), w->xy.gc, 0, 0,
          	w->core.width, w->core.height, 0, 0);
     }
     
     /* Call the redisplay callback so an application which draws on the xy
        widget can refresh it's graphics */
     if (XtIsRealized((Widget)w))
          XtCallCallbacks((Widget)w, XmNredisplayCallback, NULL);
}

void drawCrosshairs( Display* display, Drawable drawBuf,
                     XYWidget w, int x_win, int y_win ) {
     XmString string ;

     /* Draw the crosshair the first time */
     XDrawLine( display, drawBuf, w->xy.zoomGC, 
                                  w->xy.xOrigin, y_win, 
                                  w->xy.xEnd,    y_win ) ;
     XDrawLine( display, drawBuf, w->xy.zoomGC, 
                                  x_win, w->xy.yOrigin, 
                                  x_win, w->xy.yEnd ) ;

     /* Label for vertical cross hair */
     w->xy.crosshairVertXTextCoord = x_win+3 ;
     w->xy.crosshairVertYTextCoord = w->xy.yOrigin - 16 ;
     string = XmStringCreateLocalized( w->xy.crosshairVertText ) ; 
     XmStringDraw(display, drawBuf, w->xy.render_font, 
                  string, w->xy.zoomGC,
                  w->xy.crosshairVertXTextCoord, 
                  w->xy.crosshairVertYTextCoord,
                  16, XmALIGNMENT_BEGINNING,
                  XmSTRING_DIRECTION_L_TO_R, NULL);
     XmStringFree( string ) ;

     /* Label for horizontal cross hair */
     w->xy.crosshairHoriXTextCoord = w->xy.xOrigin+3 ;
     w->xy.crosshairHoriYTextCoord = y_win - 16 ;
     string = XmStringCreateLocalized( w->xy.crosshairHoriText  ) ; 
     XmStringDraw(display, drawBuf, w->xy.render_font, 
                  string, w->xy.zoomGC,
                  w->xy.crosshairHoriXTextCoord, 
                  w->xy.crosshairHoriYTextCoord,
                  16, XmALIGNMENT_BEGINNING,
                  XmSTRING_DIRECTION_L_TO_R, NULL);
     XmStringFree( string ) ;
}

/*
 ** This pops up a dialog box for the user to enter a formula 
 ** for combining curves arithmetically 
 **
 ** Keith's Addition
 ** 7-24-2003 
 */
void combineCurvesDialog(XYWidget w, XEvent *event __attribute__ ((unused)), char* args __attribute__ ((unused)), int n_args __attribute__ ((unused))) {

     int ii ;
     int nPopupChildren ;
     WidgetList popupChildren ;

     static XYManipulateCurveCBData xym ;

     Widget dialog ;
     Arg wargs[5] ;
     int nargs ;
     XmString labelTitle ;

     /* Always pass XY widget that was clicked in to CallBack */
     xym.w = w ;

     /* If there is already a dialog box up, return */
     GetWidgetPopupList(XtParent(w), &popupChildren, &nPopupChildren) ;
     for ( ii = 0 ; ii < nPopupChildren ; ii++ ) {
          if ( ! strcmp( XtName(popupChildren[ii]), 
                         "curve_formula_dialog_popup" ) )  {
               return ;
          }
     }

     labelTitle = XmStringCreateLocalized("Formula:");
     nargs = 0 ;
     XtSetArg( wargs[nargs], XmNselectionLabelString, labelTitle) ; nargs++ ;
     XtSetArg( wargs[nargs], XmNautoUnmanage, False) ; nargs++ ;
     XtSetArg( wargs[nargs], XmNtitle, "Curve Formula") ; nargs++ ;
     dialog = XmCreatePromptDialog( XtParent(w), 
                                    "curve_formula_dialog", wargs, nargs ) ; 
     XmStringFree( labelTitle ) ;

     XtAddCallback( dialog, XmNokCallback,     
                            combineCurvesOKCB,  
                            &xym) ;

     XtAddCallback( dialog, XmNcancelCallback, combineCurvesCancelCB, NULL ) ;
/*
     XtAddCallback( dialog, XmNhelpCallback,   combineCurvesHelpCB, w ) ;
*/

     XtManageChild( dialog ) ;
     XtPopup( XtParent(dialog), XtGrabNone ) ;
}

/*
 ** Callback from OK button being pressed on combineCurvesDialog 
 ** This CB will take the formula string from the dialog
 ** and call routines to actually combine the curves
 ** using the formula
 **
 ** Keith's Addition
 ** 7-24-2003 
 */
void combineCurvesOKCB( Widget parent_w, XtPointer client_data, 
                        XtPointer call_data ) {
     int ii ;
     int numCurves ;
     char* formula_xm ;
     char formula[512] ;
     XYCurve* curve ;
     XYCurve* newCurves ;
     XYWidget w ;
     XYManipulateCurveCBData* xym ;

     /* Get formula entered by user in dialog box */
     XmSelectionBoxCallbackStruct *cbs =
          (XmSelectionBoxCallbackStruct*) call_data ;
     XmStringGetLtoR( cbs->value, 
                      XmFONTLIST_DEFAULT_TAG,
                      &formula_xm ) ; 
     strcpy( formula, formula_xm ) ;
     XtFree( formula_xm ) ;

     curve = (XYCurve *) XtMalloc(sizeof(XYCurve)) ;

    /* XYWidget passed in */
    xym = (XYManipulateCurveCBData*) client_data ;
    w = xym->w ;

    newCurves = (XYCurve *)malloc(sizeof(XYCurve) * (w->xy.nCurves + 1)) ;
    numCurves = w->xy.nCurves + 1 ;

    for (ii = 0; ii < w->xy.nCurves; ii++) {
        copyCurveData(&w->xy.curves[ii], &newCurves[ii], False) ;
    }

    curve = curvemath( XtParent(parent_w), w->xy.curves, 
                       w->xy.nCurves, formula) ;

    /* curvemath() found no common points, so it returned NULL */
    if ( curve == NULL ) {


         for ( ii = 0 ; ii < numCurves-1 ; ii++ ) {
   	       freeCurveData(&newCurves[ii],False) ;
         }

         XtFree((char *) newCurves) ;

         return ;
    }

    copyCurveData(curve,&newCurves[numCurves-1],False) ;
    freeCurveData(curve,False) ;
    XtFree((char *) curve) ;

    setContents((Widget)w,newCurves,numCurves,XY_RESCALE,False) ;

    for ( ii = 0 ; ii < numCurves ; ii++ ) {
   	  freeCurveData(&newCurves[ii],False) ;
    }
    XtFree((char *) newCurves) ;
}

/*
 ** Callback from Cancel button being pressed on combineCurvesDialog 
 **
 ** Keith's Addition
 ** 7-24-2003 
 */
void combineCurvesCancelCB( Widget w, XtPointer client_data __attribute__ ((unused)), 
                            XtPointer call_data __attribute__ ((unused))) {
     XtDestroyWidget( w ) ;
}


int maxnumpts(XYCurve* curves, int numCurves)
{
     int maxnum, i ;
     maxnum = 0;
     for (i = 0; i < numCurves; i++)
     {
     	if (curves[i].nPoints > maxnum)
     	{
     		maxnum = curves[i].nPoints ;
     	}
     }
     return (maxnum) ;
}

XYCurve* curvemath( Widget parent_w, XYCurve* curves, 
                    int numCurves, char *equation)
{

#define FREE_CPI   \
            for ( i = 0 ; i < maxnumcurvepts ; i++ ) { \
	          free(commonPtIndex[i]) ; \
            } \
	     free(commonPtIndex) ; \
            free( iindex ) ;

     /* For error dialog */
     char err_msg[32] ;
     Widget dialog ;
     Arg wargs[5] ;
     int nargs ;
     XmString msg ;

     /* Colors for curves */
     Colormap colormap ;
     Display* display ;
     Screen* screen ;
     XColor xWindowsColorScreen ;
     XColor xWindowsColorExact ;
     Pixel pixel ;
     #define CURVEMATH_NUM_COLORS 14
     char colors[CURVEMATH_NUM_COLORS][32] = {
         "black", "red", "blue", "green", "magenta", 
         "yellow", "cyan", "pink", "darkgreen", "lightblue", 
         "purple", "orange", "maroon", "goldenrod" } ;

     XYCurve *result ;
     XYCurve* tempCurves ;
     char *tempeq ;
     char tempbuff[512] ;
     char strnum[16] ;
     int* iindex ;
     int maxnumcurvepts ;
     int** commonPtIndex ;
     int numCommonPts, i, j, k, l, m, n, o, p, q, r, s, t, u ;
     int numMatches , moreMatches , cPresent , colon ;
     double checkX ;
     int numUsedCurves, cnum, tcnum , rrindex, pt, tpt, spt, len ;
     int ret ;
     result = (XYCurve *) malloc(sizeof(XYCurve)) ;
     tempCurves = (XYCurve *) malloc(sizeof(XYCurve) * numCurves) ;
     memset(tempbuff,0,512) ;
     memset(strnum,0,16) ;
     tempeq = tempbuff ;
     len = strlen (equation) ;
     numCommonPts = 0 ;
     moreMatches = 0 ;
     numUsedCurves = 0 ;
     cPresent = 0 ;
     colon = 0;
     result->name = XmStringCreateLocalized(equation) ;
     result->markerStyle = curves[0].markerStyle ;
     result->markerSize = curves[0].markerSize ;
     result->lineStyle = curves[0].lineStyle ;

     /* Allocations */
     iindex = (int*) calloc ( numCurves, sizeof(int) ) ; 
     maxnumcurvepts = maxnumpts(curves, numCurves) ;
     commonPtIndex = (int**) calloc ( maxnumcurvepts, sizeof(int*) ) ;
     for ( i = 0 ; i < maxnumcurvepts ; i++ ) {
	      commonPtIndex[i] = (int*) calloc ( numCurves, sizeof(int) ) ;
     }

     /* Get new color for each new curve */
     display  = XtDisplay(parent_w) ;
     screen   = XtScreen(parent_w) ;
     colormap = DefaultColormapOfScreen(screen);
     XAllocNamedColor( display, colormap, 
                       colors[numCurves%CURVEMATH_NUM_COLORS],
                       &xWindowsColorScreen,
                       &xWindowsColorExact ) ;
     pixel = xWindowsColorScreen.pixel ;
     result->linePixel = pixel ; 
     result->markerPixel = pixel ;

     result->nPoints = 0 ;
     spt = 0 ;
    
     /* Set defaults in result curve (valgrind) */
     result->horizBars = NULL ;
     result->vertBars  = NULL ;

     for (r = 0; r < numCurves; r++)
     {
     	iindex[r] = 0 ;
     }
     for (s = 0; s < maxnumcurvepts; s++)
     {
     	for (t = 0; t < numCurves; t++)
     	{
     		commonPtIndex[s][t] = 0 ;
     	}
     }

     for (p = 0; p < (len-1); p++)
     {
	     if ((equation[p] == 'c') && isdigit(equation[p+1]) )
	     {
		     cPresent = 1 ;
		     break ;
	     }
     }
     if (cPresent != 1)
     {
	     char *tequation ;
	     char teqbuff[64] ;
	     char prefix[64] = "c0:" ;
	     char *tprefix = prefix ;
	     memset(teqbuff,0,64) ;
	     tequation = teqbuff ;
	     tequation = strcat(tprefix,equation) ;
	     strcpy(equation,tequation) ;
     }
     len = strlen (equation) ;
     for (p = 0; p < (len-1); p++)
     {
	     if (equation[p] == ':')
	     {
		     char *curveStr ;
		     char csbuff[64] ;
		     char *curveNumStr ;
		     char cnsbuff[64] ;
		     memset(csbuff,0,64) ;
		     memset(cnsbuff,0,64) ;
		     curveStr = csbuff ;
		     curveNumStr = cnsbuff ;
		     colon = 1 ;
		     strncpy(curveStr,equation,p) ;
		     curveNumStr = &curveStr[1] ;
		     equation = &equation[p+1] ;
		     cnum = atoi(curveNumStr) ;
		     break ;
	     }
     }
     len = strlen (equation) ;
     if (colon == 1)
     {
	     result->points = (XYPoint *) malloc 
	                      (sizeof(XYPoint) * (curves[cnum].nPoints)) ;
	     numCommonPts = curves[cnum].nPoints ;
	     result->nPoints = curves[cnum].nPoints ;
	     for ( i = 0 ; i < curves[cnum].nPoints ; i++ ) {
		     result->points[i].x = curves[cnum].points[i].x ;
		     commonPtIndex[i][0] = i ;
	     }
	     for ( i = 0 ; i < curves[cnum].nPoints ; i++ ) {
		     ret = equationparse(equation,
                                       result->points[i].x,
                                       &result->points[i].y) ;
                   if ( ret ) {
                        errorEQParseDialog( parent_w, ret ) ;
	                 XtFree((char *)tempCurves) ;
			   FREE_CPI
	                 return (NULL) ;
                   }
	     }
	     XtFree((char *)tempCurves) ;
            FREE_CPI ;
	     return (result) ;
     }

     for (p = 0; p < (len-1); p++)
     {
     	if ((equation[p] == 'c') && isdigit(equation[p+1]))
     	{
     		cnum = 0 ;
     		p++ ;
     		spt = 0 ;
     		memset(strnum,0,16) ;
     		while ( isdigit(equation[p])  && (p < len) )
     		{
     			strnum[spt] = equation[p] ;
     			spt++ ;
     			p++ ;
     		}
     		cnum = atoi(strnum) ;
     		if (cnum >= numCurves)
     		{
                   /* Error dialog popup */
                   sprintf(err_msg,"%d is not a valid curve number", cnum) ;
                   msg = XmStringCreateLocalized(err_msg) ;
                   nargs = 0 ;
                   XtSetArg( wargs[nargs], XmNmessageString, msg ) ; nargs++ ;
                   XtSetArg( wargs[nargs], XmNtitle, "Error" ) ; nargs++ ;
                   dialog = XmCreateErrorDialog( parent_w, 
                                                 "error_message", 
                                                 wargs, nargs ) ;
                   XtManageChild( dialog ) ;

                   FREE_CPI;
                   return(NULL);
     		}
     		copyCurveData(&curves[cnum],&tempCurves[numUsedCurves],False) ;
     		numUsedCurves++ ;
     	}
     }

     result->points = (XYPoint *) malloc 
                      (sizeof(XYPoint) * (tempCurves[0].nPoints)) ;

     
     for (i = 0; i < tempCurves[0].nPoints; i++)
     {
     	for (m = 0; m < numUsedCurves; m++)
     	{
     		if (iindex[m] == tempCurves[m].nPoints)
     		{
     			moreMatches = 1 ;
     			break ;
     		}
     	}
     	if (moreMatches == 1)
     	{
     		break ;
     	}
     	numMatches = 0 ;
     	checkX = tempCurves[0].points[i].x ;
     	for (j = 1; j < numUsedCurves; j++)
     	{
     		for (k = 0; k < tempCurves[j].nPoints; k++)
     		{
     			if (tempCurves[j].points[iindex[j]].x > checkX)
     			{
     				break ;
     			}
     			else if (tempCurves[j].points[iindex[j]].x == checkX)
     			{
     				numMatches++ ;
     				break ;
     			}
     			else
     			{
     				iindex[j] = iindex[j] + 1 ;
     			}
     		}
     		if (numMatches < j)
     		{
     			break ;
     		}
     		else if (numMatches == (numUsedCurves - 1))
     		{
     			/*commonPts[numCommonPts] = checkX ;*/
     			for (o = 0; o < numUsedCurves; o++)
     			{
     				commonPtIndex[numCommonPts][o] = iindex[o] ;
     			}
     			result->points[result->nPoints].x = 
			                                tempCurves[0].points[i].x ;
     			result->nPoints++ ;
     			numCommonPts++ ;
     			for (l = 1; l < numUsedCurves; l++)
     			{
     				iindex[l] = iindex[l] + 1 ;
     			}
     		}
     	}
     	iindex[0] = iindex[0] + 1 ;
     }

     if ( numUsedCurves == 1 ) {
          numCommonPts = tempCurves[0].nPoints ;
          result->nPoints = tempCurves[0].nPoints ;
          for ( i = 0 ; i < tempCurves[0].nPoints ; i++ ) {
               result->points[i].x = tempCurves[0].points[i].x ;
               commonPtIndex[i][0] = i ;
          }
     }

     if (numCommonPts == 0)
     {
       free( result ) ;
       XtFree((char *)tempCurves) ;

       /* Warning dialog box that says no common points */ 
       msg = XmStringCreateLocalized( 
               "When combining curves there were\n"
               "no points between the curves where\n"
               "the time stamp matched.\n"
               "So there is nothing to display." ); 
       nargs = 0 ;
       XtSetArg( wargs[nargs], XmNmessageString, msg ) ; nargs++ ;
       XtSetArg( wargs[nargs], XmNtitle, "Warning" ) ; nargs++ ;
       dialog = XmCreateWarningDialog( parent_w, "warning_message",
                                         wargs, nargs ) ;
       XtManageChild( dialog ) ;

       FREE_CPI ;
     	return (NULL);
     }
     
     rrindex = 0;
     while (rrindex < numCommonPts)
     {
     	memset(tempbuff,0,512) ;
     	tempeq = tempbuff ;
     	pt = 0 ;
     	tpt = 0 ;
     	while( pt < len )
     	{
              if ( pt == len - 1 ) {
     			tempeq[tpt] = equation[pt] ;
     			tpt++ ;
     			pt++ ;
              } 
              else if ((equation[pt] == 'c') && (equation[pt+1] >= 48) 
                                             && (equation[pt+1] <= 57))
     		{
     			pt++ ;
     			n = pt ;
     			cnum = 0 ;
     			spt = 0 ;
     			memset(strnum,0,16) ;
     			while ((equation[n] >= 48) && (equation[n] <= 57) && (n < len))
     			{
     				strnum[spt] = equation[n] ;
     				spt++ ;
     				pt++ ;
     				n++ ;
     			}
     			cnum = atoi(strnum) ;
     			tcnum = -1 ;
     			for (q = 0; q < numUsedCurves; q++)
     			{
     				if (XmStringCompare(curves[cnum].name, tempCurves[q].name))
     				{
     					tcnum = q ;
     					break ;
     				}
     			}
     			if (tcnum == -1)
     			{
     				printf("\n Parse error in XY.c, no match found...") ;
     				exit(1) ;
     			}
     			if (curves[cnum].points[commonPtIndex[rrindex][tcnum]].y <= 0)
     			{
     				sprintf(tempeq,"%s(%.12lf)",
                                tempeq,
			           curves[cnum].points[
                                       commonPtIndex[rrindex][tcnum]].y) ;
     			}
     			else
     			{
     				sprintf(tempeq,"%s%.12lf",
                                       tempeq,
                                       curves[cnum].points[
                                              commonPtIndex[rrindex][tcnum]].y);
     			}
     			tpt = strlen (tempeq) ;
     		}
     		else
     		{
     			tempeq[tpt] = equation[pt] ;
     			tpt++ ;
     			pt++ ;
     		}
     	}
#if 0
     	printf("\nEquation after substitution: %s",tempeq) ;
#endif
     	
     	ret  = equationparse(tempeq,
                            result->points[rrindex].x,
                            &result->points[rrindex].y) ;
       if ( ret ) {
            errorEQParseDialog( parent_w, ret ) ;
            break ;
       }

     	rrindex++ ;
     }

     for (u = 0; u < numUsedCurves; u++)
     {
     	freeCurveData(&tempCurves[u],False) ;
     }
     XtFree((char *)tempCurves) ;

     FREE_CPI ;
     if ( ret ) {
           /* Error has occured */
           return( NULL ) ;
     } else {
           return (result) ;
     }
}

XYCurve* curvesub(XYCurve* c1, XYCurve* c2)
{
     int c1index, c2index, rrindex ;
     XYCurve *result ;
     XmString nameSep ;
     XmString tmpString ;
     result = (XYCurve *) malloc(sizeof(XYCurve)) ;
     nameSep = XmStringCreateLocalized (" - ") ;
     c1index = 0 ;
     c2index = 0 ;
     rrindex = 0 ;
     tmpString    = XmStringConcat(c1->name, nameSep) ;
     result->name = XmStringConcat(tmpString, c2->name) ;
     XmStringFree(nameSep) ;
     result->markerStyle = c1->markerStyle ;
     result->markerSize = c1->markerSize ;
     result->lineStyle = c1->lineStyle ;
     result->markerPixel = c1->markerPixel ;
     result->linePixel = c1->linePixel ;
     result->nPoints = 0 ;
     
     if (c1->nPoints < c2->nPoints)
     {
            result->points = (XYPoint *) malloc
                             (sizeof(XYPoint) * (c1->nPoints)) ;
     }
     else
     {
     	result->points = (XYPoint *) malloc
                               (sizeof(XYPoint) * (c2->nPoints)) ;
     }
     
     while ((c1index < c1->nPoints) && (c2index < c2->nPoints))
     {
     	if (c1->points[c1index].x < c2->points[c2index].x)
     	{
     		c1index++ ;
     	}
     	else if (c1->points[c1index].x > c2->points[c2index].x)
     	{
     		c2index++ ;
     	}
     	else if (c1->points[c1index].x == c2->points[c2index].x)
     	{
     		result->nPoints++ ;
     		result->points[rrindex].x = c1->points[c1index].x ;
     		result->points[rrindex].y = c1->points[c1index].y - 
                                                c2->points[c2index].y ;

     		rrindex++ ;
     		c1index++ ;
     		c2index++ ;
     	}
     }

     return (result) ;
}

void errorEQParseDialog( Widget w, int error_code ) {
  
     char* error_msg ;
                                                                                
     /* For error dialog */
     Widget dialog ;
     Arg wargs[5] ;
     int nargs ;
     char msg1[80] ;
     XmString msg ;
                                                                                
     error_msg = eqperror( error_code ) ;
     sprintf(msg1, "%s%s", "An error was encountered while handling\n"
                           "the formula entry.\n\n", 
                           error_msg) ;
     free( error_msg ) ;

     /* Warning dialog box that says no common points */
     msg = XmStringCreateLocalized(msg1) ;
     nargs = 0 ;
     XtSetArg( wargs[nargs], XmNmessageString, msg ) ; nargs++ ;
     XtSetArg( wargs[nargs], XmNtitle, "Error" ) ; nargs++ ;
     dialog = XmCreateErrorDialog( w, "error_message", wargs, nargs ) ;
     XtManageChild( dialog ) ;
                                                                                
   return;
}

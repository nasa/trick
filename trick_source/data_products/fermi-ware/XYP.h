/*******************************************************************************
*									       *
* XYP.h - General Purpose Plot Widget, Private Header File			       *
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
#ifndef XYP_H
#define XYP_H

#include "XY.h"
#include <Xm/XmP.h>
#if XmVERSION >= 2
#include <Xm/PrimitiveP.h>
#endif

typedef struct _XYClassPart{
    int ignore;
} XYClassPart;

typedef struct _XYClassRec{
    CoreClassPart  core_class;
    XmPrimitiveClassPart primitive_class;
    XYClassPart  xy_class;
} XYClassRec;

extern XYClassRec xyClassRec;

typedef struct _XYPart {
                                /* JBF - added zoomGC from old version */
    GC zoomGC;                  /* Graphics context for zoom box */
    GC gc;   	                  /* Graphics context for axes & labels */
    GC contentsGC;              /* Graphics context for plot contents */
    Pixmap drawBuffer;          /* Double buffering for non-flashing draws */

    Pixmap valueBuffer;         /* Double buffering for value mark */
    int valueX ;                /* coordinates of mark */
    int valueY ;                /* coordinates of mark */

    int xMin, yMin, xMax, yMax; /* Boundaries of the drawable area of widget */
#if XmVERSION < 2
    XmFontList render_font;     /* Motif font list associated with widget */
#else
    XmRenderTable render_font;  /* Motif Render Table associated with widget */
#endif
    XtCallbackList resize;	/* Callbacks */
    XtCallbackList btn2;
    XtCallbackList btn3;
    XtCallbackList redisplay;
    Boolean doubleBuffer;	/* When set, draw first to offscreen pixmap */
    Boolean xLogScaling;	/* When set, plot X axis as log of X */
    Boolean yLogScaling;	/* When set, plot Y axis as log of Y */
    Boolean showLegend;     /* When set, show the plot legend */
    Boolean showGrid;       /* When set, draw the grid lines */

    Pixel gridColor;        /* color for grid lines */
    Pixel printGridColor;   /* color for grid lines */
                            /* JBF - added plotTitle from old version */
    XmString plotTitle;     /* Compound string for plot title */


    XmString xAxisLabel;	/* Compound string labels for axes */
    XmString yAxisLabel;
    XmString yAxisFormat;   /* printf format to show on Y axis */
    int marginPercent;      /* Size of plot border in % of data range */
    int xOrigin, yOrigin;	/* The point where the axis lines meet */
    int xEnd, yEnd;		/* The ends of the x and y axis lines */
    int legendTop;		/* Y coord. of the top of the plot legend */
    int legendLeftMargin;	/* left edge of legend */
    int axisLeft, axisTop;	   /* Along with xOrigin and yOrigin, define */
    int axisBottom, axisRight; /* the boundaries of the axis areas */
    int legendRows;		 /* Number of rows in the plot legend */
    int legendColumnSpacing; /* How far apart to space legend columns */
    int dragState;		 /* Is the user currently dragging the mouse? */
    double xDragStart;       /* X (data coord) position of start of drag */
    double yDragStart;	 /* Y (data coord) position of start of drag */
    double minXData, maxXData;	/* Minimum and maximum x data values */
    double minYData, maxYData;	/* Minimum and maximum y data values */
    double minXLim, maxXLim;	/* Min and max x data actually displayed */
    double minYLim, maxYLim;	/* Min and max y data actually displayed */
    XYCurve *curves;         /* Data to be displayed on the plot */
    int nCurves;             /* Number of curves in curves above */
    XYString *strings;       /* Text strings to be displayed on the plot */
    int nStrings;            /* Number of text strings in strings above */
    int selectedCurve ;      /* Index of curve selected by user */

    Boolean enableCrosshairs ;    /* User preference */
    int crosshairFirst ;          /* First time to display crosshair */
    int lastCrosshairX ;          /* Save X-Y pixel location of crosshair */
    int lastCrosshairY ;         
    int crosshairHoriXTextCoord ; /* X-Y pix location of value on horizontal */
    int crosshairHoriYTextCoord ; /* line in cross hair */
    int crosshairVertXTextCoord ; /* X-Y pix location of value on vertical */
    int crosshairVertYTextCoord ; /* line in cross hair */
    char crosshairHoriText[16] ;  /* Numbers displayed for crosshair */
    char crosshairVertText[16] ; 

    Boolean valueDisplay ;    /* User preference */

} XYPart;

typedef struct _XYRec {
   CorePart        core;
   XmPrimitivePart primitive;
   XYPart        xy;
} XYRec;

#endif

/*******************************************************************************
*									       *
* dragAxes.c - Handle axis dragging for 2D plotting widgets		       *
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
#include <math.h>
#include <X11/Xlib.h>
#include "dragAxes.h"

enum dragStates {NOT_DRAGGING, DRAGGING_NOTHING, DRAGGING_TOP, DRAGGING_BOTTOM,
	DRAGGING_BOTTOM_AND_LEFT, DRAGGING_LEFT, DRAGGING_RIGHT, DRAGGING_DATA};

/*
** ResetAxisDragging
**
** Call this before calling DragAxes to initialize the drag state.  Can also
** be called to stop any axis dragging currently in progress.
*/
void ResetAxisDragging(int *dragState)
{
    *dragState = NOT_DRAGGING;
}

/*
** DragAxes
**
** process a button motion (mouse drag) event to 1) check if it is within
** the boundaries of the axis dragging areas and 2) if so process the
** drag event to adjust the plotting limits.  Requires the caller to
** maintain the dragState, xDragStart, and yDragStart variables between
** calls.  Call ResetAxisDragging to initialize dragState before calling
** this routine.
*/
int DragAxes(XEvent *event, int xOrigin, int xEnd, int yOrigin,
	int yEnd, int axisLeft, int axisTop, int axisBottom, int axisRight,
	double minXData, double maxXData, double minYData, double maxYData,
	int xLogScaling, int yLogScaling, double *minXLimit, double *maxXLimit,
	double *minYLimit, double *maxYLimit, int *dragState,
	double *xDragStart, double *yDragStart)
{
    double minXLim, minYLim, maxXLim, maxYLim;
    double minXDat, minYDat, maxXDat, maxYDat;
    double xScale, yScale, xOffset, yOffset, newLim;
    int x, y;
    int redrawArea = DA_REDRAW_NONE;

    /* use log coordinates (which are linear) for log scaling on either axis */
    if (xLogScaling) {
    	minXLim = log10(*minXLimit); maxXLim = log10(*maxXLimit);
    	minXDat = log10(minXData); maxXDat = log10(maxXData);
    } else {
    	minXLim = *minXLimit; maxXLim = *maxXLimit;
    	minXDat = minXData; maxXDat = maxXData;
    }
    if (yLogScaling) {
    	minYLim = log10(*minYLimit); maxYLim = log10(*maxYLimit);
    	minYDat = log10(minYData); maxYDat = log10(maxYData);
    } else {
    	minYLim = *minYLimit; maxYLim = *maxYLimit;
    	minYDat = minYData; maxYDat = maxYData;
    }
    
    /* calculate scale factors for translating data coords to pixel coords */
    xScale = (maxXLim - minXLim)/(xEnd - xOrigin);
    yScale = (maxYLim - minYLim)/(yOrigin - yEnd);
    
    if (event->type == ButtonPress || event->type == MotionNotify) {
	x = event->xbutton.x;
	y = event->xbutton.y;
    	switch (*dragState) {
    	  case NOT_DRAGGING:
    	    if (x >= xOrigin && x <= xEnd && y <= yOrigin && y >= yEnd)
    	    	*dragState = DRAGGING_DATA;
    	    else if (x >= axisLeft && x <= xOrigin &&
    	    	     y >= yOrigin && y <= axisBottom)
    	    	*dragState = DRAGGING_BOTTOM_AND_LEFT;
    	    else if (x >= axisLeft && x <= xOrigin &&
    	    	     y >= axisTop && y <= axisBottom)
    	    	if (y < yEnd + (yOrigin - yEnd)/2)
    	    	    *dragState = DRAGGING_TOP;
    	    	else
    	   	    *dragState = DRAGGING_BOTTOM;
    	    else if (x >= axisLeft && x <= axisRight &&
    	    	     y <= axisBottom && y >= yOrigin    )
    	    	if (x < xOrigin + (xEnd - xOrigin)/2)
    	    	    *dragState = DRAGGING_LEFT;
    	    	else
    	   	    *dragState = DRAGGING_RIGHT;
    	    else
    	    	*dragState = DRAGGING_NOTHING;
    	    *xDragStart = minXLim + (double)(x - xOrigin) * xScale;
    	    *yDragStart = minYLim + (double)(yOrigin - y) * yScale;
    	    break;
    	  case DRAGGING_NOTHING:
    	    break;
    	  case DRAGGING_DATA:
    	    xOffset = minXLim - (*xDragStart - (x - xOrigin) * xScale);
    	    yOffset = minYLim - (*yDragStart - (yOrigin - y) * yScale);
    	    if (maxXLim - xOffset > maxXDat)
    	    	xOffset = maxXLim - maxXDat;
    	    else if (minXLim - xOffset < minXDat)
    	    	xOffset = minXLim - minXDat;
    	    if (xOffset != 0.) {
   		*maxXLimit = xLogScaling ? pow(10., maxXLim-xOffset) :
   						maxXLim-xOffset;
    		*minXLimit = xLogScaling ? pow(10., minXLim-xOffset) :
    						minXLim-xOffset;
    	    	redrawArea |= DA_REDRAW_H_AXIS | DA_REDRAW_CONTENTS;
    	    }
    	    if (maxYLim - yOffset > maxYDat)
    	    	yOffset = maxYLim - maxYDat;
    	    else if (minYLim - yOffset < minYDat)
    	    	yOffset = minYLim - minYDat;
    	    if (yOffset != 0.) {
   		*maxYLimit = yLogScaling ? pow(10., maxYLim-yOffset) :
   						maxYLim-yOffset;
    		*minYLimit = yLogScaling ? pow(10., minYLim-yOffset) :
    						minYLim-yOffset;
    	    	redrawArea |= DA_REDRAW_V_AXIS | DA_REDRAW_CONTENTS;
    	    }
    	    break;
    	  case DRAGGING_TOP:
    	    if (y < yOrigin)
    	    	newLim = minYLim + (*yDragStart - minYLim) *
    	    		  	   (double)(yOrigin-yEnd)/(double)(yOrigin-y);
    	    if (y >= yOrigin || newLim > maxYDat)
    	    	newLim = maxYDat;
    	    if (newLim != maxYLim)
    	    	redrawArea |= DA_REDRAW_V_AXIS | DA_REDRAW_CONTENTS;
    	    *maxYLimit = yLogScaling ? pow(10., newLim) : newLim;
    	    break;
    	  case DRAGGING_BOTTOM:
    	    if (y > yEnd)
    	    	newLim = maxYLim - (maxYLim - *yDragStart) *
    	    	 		   (double)(yOrigin - yEnd)/(double)(y - yEnd); 
    	    if (y <= yEnd || newLim < minYDat)
    	    	newLim = minYDat;
    	    if (newLim != minYLim)
    		redrawArea |= DA_REDRAW_V_AXIS | DA_REDRAW_CONTENTS;
    	    *minYLimit = yLogScaling ? pow(10., newLim) : newLim;
    	    break;
    	  case DRAGGING_BOTTOM_AND_LEFT:
    	    if (y > yEnd)
    	    	newLim = maxYLim - (maxYLim - *yDragStart) *
    	    	 		   (double)(yOrigin - yEnd)/(double)(y - yEnd); 
    	    if (y <= yEnd || newLim < minYDat)
    	    	newLim = minYDat;
    	    if (newLim != minYLim)
    		redrawArea |= DA_REDRAW_V_AXIS | DA_REDRAW_CONTENTS;
    	    *minYLimit = yLogScaling ? pow(10., newLim) : newLim;
    	    if (x < xEnd)
    	    	newLim = maxXLim - (maxXLim - *xDragStart) *
    	    	 		   (double)(xEnd - xOrigin)/(double)(xEnd - x);
   	    if (x >= xEnd || newLim < minXDat)
   	    	newLim = minXDat;
   	    if (newLim != minXLim)
   	    	redrawArea |= DA_REDRAW_H_AXIS | DA_REDRAW_CONTENTS;
   	    *minXLimit = xLogScaling ? pow(10., newLim) : newLim;
    	    break;
    	  case DRAGGING_RIGHT:
    	    if (x > xOrigin)
    	    	newLim = minXLim + (*xDragStart - minXLim) *
    	    	 	 (double)(xEnd - xOrigin)/(double)(x - xOrigin);
    	    if (x <= xOrigin || newLim > maxXDat)
    	    	newLim = maxXDat;
    	    if (newLim != maxXLim)
    	    	redrawArea |= DA_REDRAW_H_AXIS | DA_REDRAW_CONTENTS;
    	    *maxXLimit = xLogScaling ? pow(10., newLim) : newLim;
    	    break;
    	  case DRAGGING_LEFT:
    	    if (x < xEnd)
    	    	newLim = maxXLim - (maxXLim - *xDragStart) *
    	    	 		   (double)(xEnd - xOrigin)/(double)(xEnd - x);
   	    if (x >= xEnd || newLim < minXDat)
   	    	newLim = minXDat;
   	    if (newLim != minXLim)
   	    	redrawArea |= DA_REDRAW_H_AXIS | DA_REDRAW_CONTENTS;
   	    *minXLimit = xLogScaling ? pow(10., newLim) : newLim;
    	    break;
    	}
    }
    
    /* conversion back and forth to log coordinates can introduce error which
       will make the plot limits not match the data limits when the user has
       dragged the scale all the way back in.  This code restores them */
    if (xLogScaling) {
	minXLim = log10(*minXLimit); maxXLim = log10(*maxXLimit);
	if (fabs(minXLim - minXDat) < (maxXLim - minXLim) / 1000.)
    	    *minXLimit = minXData;
	if (fabs(maxXLim - maxXDat) < (maxXLim - minXLim) / 1000.)
    	    *maxXLimit = maxXData;
    }
    if (yLogScaling) {
	minYLim = log10(*minYLimit); maxYLim = log10(*maxYLimit);
	if (fabs(minYLim - minYDat) < (maxYLim - minYLim) / 1000.)
    	    *minYLimit = minYData;
	if (fabs(maxYLim - maxYDat) < (maxYLim - minYLim) / 1000.)
    	    *maxYLimit = maxYData;
    }
    
    return redrawArea;
}

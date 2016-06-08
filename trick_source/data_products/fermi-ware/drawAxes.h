/*******************************************************************************
*									       *
* drawAxes.h -- Generic axis drawing routines for 2D graphs & plots	       *
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
int HAxisHeight(XFontStruct *fs);
int HAxisEndClearance(XFontStruct *fs);
int VAxisWidth(XFontStruct *fs, XmString format);
int VAxisEndClearance(XFontStruct *fs);

void DrawHorizontalAxis(Display *display, Drawable window, GC gc,
        XFontStruct *fs, int outDevice, int y, int y2, int x1, int x2,
        double minData, double maxData, double minLimit, double maxLimit,
        int logScaling, int nBins,
	 int showGrid, Pixel gridColor, Pixel printGridColor );

void DrawVerticalAxis(Display *display, Drawable window, GC gc, XFontStruct *fs,
        int outDevice, int x, int x2, int y1, int y2, double minData, double maxData,
        double minLimit, double maxLimit, int logScaling, XmString yAxisFormat ,
	 int showGrid, Pixel gridColor, Pixel printGridColor );

void RedrawHAxisArrows(Display *display, Drawable window, GC gc,
		       int y, int x1, int x2, double minData, double maxData,
		       double minLimit, double maxLimit);
void RedrawVAxisArrows(Display *display, Drawable window, GC gc,
		       int x, int y1, int y2, double minData, double maxData,
		       double minLimit, double maxLimit);

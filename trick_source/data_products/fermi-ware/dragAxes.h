/*******************************************************************************
*									       *
* dragAxes.h - Handle axis dragging for 2D plotting widgets		       *
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
#define DA_REDRAW_NONE 0
#define DA_REDRAW_H_AXIS 1
#define DA_REDRAW_V_AXIS 2
#define DA_REDRAW_CONTENTS 4

void ResetAxisDragging(int *dragState);
int DragAxes(XEvent *event, int xOrigin, int xEnd, int yOrigin,
	int yEnd, int axisLeft, int axisTop, int axisBottom, int axisRight,
	double minXData, double maxXData, double minYData, double maxYData,
	int xLogScaling, int yLogScaling, double *minXLimit, double *maxXLimit,
	double *minYLimit, double *maxYLimit, int *dragState,
	double *xDragStart, double *yDragStart);

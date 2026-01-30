/********************************* TRICK HEADER *******************************
PURPOSE: ( Pool bumper class. )
LIBRARY DEPENDENCY:
    ((bumper.o))
*******************************************************************************/
#ifndef _bumper_hh_
#define _bumper_hh_

#include "common_geometry.hh"
#include <vector>

class Bumper
{
public:
    // Have to have a default constructor or trick freaks out
    Bumper();
    Bumper(int numPoints, double x1, double y1, double x2, double y);

    void AddPointToRender(double x, double y);
    void AddBorder(double x1, double y1, double x2, double y2);
    int id;
    Line border;
    Vec ** renderedShape;
    unsigned int numPoints;
    enum PolygonType shapeType;

private:
    int nextPointSlot = 0;
};

#endif

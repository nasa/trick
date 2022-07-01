/********************************* TRICK HEADER *******************************
PURPOSE: ( Pool bumper class. )
LIBRARY DEPENDENCY:
    ((bumper.o))
*******************************************************************************/
#ifndef _bumper_hh_
#define _bumper_hh_

#include <vector>

// this should definitely go somewhere else
// maybe make a geometry libary
class Point {
    public:
        double x;
        double y;

        Point(double x, double y) : x(x), y(y) {}
        Point() {}
};

class Line {
    public:
        Point p1;
        Point p2;

        Line (Point p1, Point p2) : p1(p1), p2(p2) {}
        Line () {}
};

class Bumper {
    public:
        void AddPointToRender(double x, double y);
        void AddBorder (double x1, double y1, double x2, double y2);

    private:
        // Actual line that can be collided with 
        Line border;

        // Shape that should be rendered
        // Size should be dynamic
        std::vector<Point *> renderedShape;
    
};

#endif
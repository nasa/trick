/*
 * PURPOSE: (Class to represent a point)
 */
#ifndef POINT_HH
#define POINT_HH
class Point {
    public:
    double x;
    double y;
    Point(): x(0.0),y(0.0) {}
    Point(double X, double Y): x(X),y(Y) {}
};
#endif

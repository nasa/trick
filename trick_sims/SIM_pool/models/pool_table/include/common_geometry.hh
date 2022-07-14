/********************************* TRICK HEADER *******************************
PURPOSE: ( Geometry primitives to be used elsewhere. )
LIBRARY DEPENDENCY:
    (())
*******************************************************************************/
#ifndef _common_hh_
#define _common_hh_

// Should maybe swap this for eigen stuff at some point
class Vec {
    public:
        Vec(double x, double y) : _x(x), _y(y), _z(0) {}
        Vec(double x, double y, double z) : _x(x), _y(y), _z(z) {}
        Vec() {}

        Vec operator+ (const Vec& other) {
            Vec sum;
            sum._x = _x + other._x;
            sum._y = _y + other._y;
            sum._z = _z + other._z;
            return sum;
        }

        Vec operator* (double scale) {
            Vec ret;
            ret._x = _x * scale;
            ret._y = _y * scale;
            ret._z = _z * scale;
            return ret;
        }

        double& operator() (int index) {
            if (index == 0) {
                return _x;
            } else if (index == 1) {
                return _y;
            } else if (index == 2) {
                return _z;
            }

            // Throw an error i guess
        }

        Vec normalized () {
            Vec norm;
            
        }

        double& x () { return _x; }
        double& y () { return _y; }
        double& z () { return _z; }


    private:
        double _x;
        double _y;
        double _z;
};

class Line {
    public:
        Vec p1;
        Vec p2;

        Line (Vec p1, Vec p2) : p1(p1), p2(p2) {}
        Line () {}
};

enum PolygonType {
  GENERIC,
  CIRCLE,
  TRIANGLE,
  RECTANGLE,
  QUAD
};

#endif
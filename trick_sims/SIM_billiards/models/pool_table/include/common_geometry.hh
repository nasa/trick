/********************************* TRICK HEADER *******************************
PURPOSE: ( Geometry primitives to be used elsewhere. )
LIBRARY DEPENDENCY:
    (())
*******************************************************************************/
#ifndef _common_hh_
#define _common_hh_

#include "iostream"
#include "math.h"

// Should maybe swap this for eigen stuff at some point
class Vec
{
public:
    Vec(double x, double y)
        : _x(x),
          _y(y),
          _z(0)
    {
    }

    Vec(double x, double y, double z)
        : _x(x),
          _y(y),
          _z(z)
    {
    }

    Vec()
        : _x(0),
          _y(0),
          _z(0)
    {
    }

    Vec operator+(const Vec & other)
    {
        Vec sum;
        sum._x = _x + other._x;
        sum._y = _y + other._y;
        sum._z = _z + other._z;
        return sum;
    }

    Vec operator-(const Vec & other)
    {
        Vec sum;
        sum._x = _x - other._x;
        sum._y = _y - other._y;
        sum._z = _z - other._z;
        return sum;
    }

    Vec operator*(double scale)
    {
        Vec ret;
        ret._x = _x * scale;
        ret._y = _y * scale;
        ret._z = _z * scale;
        return ret;
    }

    double dot(const Vec & other)
    {
        double ret = 0;
        ret += _x * other._x;
        ret += _y * other._y;
        ret += _z * other._z;

        return ret;
    }

    // Dot product
    double operator*(const Vec & other)
    {
        return dot(other);
    }

    double & operator[](int index)
    {
        if(index == 0)
        {
            return _x;
        }
        else if(index == 1)
        {
            return _y;
        }
        else if(index == 2)
        {
            return _z;
        }

        // Throw an error i guess
    }

    void setZero()
    {
        _x = 0;
        _y = 0;
        _z = 0;
    }

    double norm()
    {
        return sqrt(_x * _x + _y * _y + _z * _z);
    }

    Vec normalized()
    {
        Vec ret(*this);

        return ret * (1.0 / this->norm());
    }

    double & x()
    {
        return _x;
    }

    double & y()
    {
        return _y;
    }

    double & z()
    {
        return _z;
    }

    double _x;
    double _y;
    double _z;

private:
};

class Line
{
public:
    Vec p1;
    Vec p2;

    Line(Vec p1, Vec p2)
        : p1(p1),
          p2(p2)
    {
    }

    Line() {}
};

enum PolygonType
{
    GENERIC,
    CIRCLE,
    TRIANGLE,
    RECTANGLE,
    QUAD
};

#endif

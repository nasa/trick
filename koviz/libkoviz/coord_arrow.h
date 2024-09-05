#ifndef COORDARROW_H
#define COORDARROW_H

#include <QPolygonF>
#include <QPainter>
#include <QRectF>
#include <QRect>
#include <QTransform>
#include <QColor>
#include <float.h>
#include <math.h>

// ---------------------------------------------------------
// Use . since backslash makes a C++ multi-line comment
//
//               b
//         Y...........|<-m->(x,y)
//        .
//     a .
//      .
//    |_
//   . -----------------------------Z
//
//      Quad1 - angle(Y.Z)===angle===45degrees
//
//                b
// (x,y)<-m->|..........Y
//                       .
//                      a .
//                         .
//                          _|
//                            .   --------------------Z
//
//      Quad2 - angle(Y.Z)===angle===135degrees
//
// ---------------------------------------------------------
//
//            A
//            |.
//            | .
//            |  .
//            |   .
//            |    .
//            |<-h->.       Arrow Head
//            |    .
//            |   .
//            |  .
//            | .
//            |.
//            B     angle(A.B)===tipAngle===arrowTipAngle===22.5
//
// ---------------------------------------------------------
class CoordArrow
{
  public:
    CoordArrow();
    CoordArrow(const QPointF& coord,
               double r, double h,
               double a, double b, double m,
               double angle, double tipAngle);

  public:
    QString txt;    // Text e.g. "(10.375,3.141593)"
    QPointF coord;  // math coord
    double r;       // radius of circle in window coords
    double h;       // height of arrow head in window coords
    double a;       // length of part1 of tail (see above)
    double b;       // length of part2 of tail (see above)
    double m;       // dist between text box and 'b'
    double angle;   // angle of arrow off of horizon
    double tipAngle; // tip of arrow angle (22.5)

    QRectF boundingBox(const QPainter &painter, const QTransform &T) const;
    void paintMe(QPainter &painter, const QTransform &T,
                 const QColor &fg, const QColor &bg) const;
    void paintMeCenter(QPainter &painter, const QTransform &T,
                       const QRect& viewportRect,
                       const QColor &fg, const QColor &bg) const;
};

#endif // COORDARROW_H

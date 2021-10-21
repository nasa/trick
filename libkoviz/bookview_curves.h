#ifndef CURVESVIEW_H
#define CURVESVIEW_H

#include <QtGlobal>
#include <QVector>
#include <QPolygonF>
#include <QPainter>
#include <QPixmap>
#include <QList>
#include <QColor>
#include <QMouseEvent>
#include <QRubberBand>
#include <QFocusEvent>
#include <QKeyEvent>
#include <QSizeF>
#include <QLineF>
#include <QItemSelectionModel>
#include <QEasingCurve>
#include <QImage>
#include <QFontMetrics>
#include <QPoint>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include "bookidxview.h"
#include "unit.h"
#include "curvemodel.h"
#include "roundoff.h"
#include "layoutitem_curves.h"

class TimeAndIndex
{
  public:
    TimeAndIndex(double time, int timeIdx, const QModelIndex& modelIdx);
    double time() const;
    int timeIdx() const;
    QModelIndex modelIdx() const;

  private:
    TimeAndIndex() {}
    double _time;
    int _timeIdx;  // Index for duplicate timestamps
    QModelIndex _modelIdx;
};

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

class CurvesView : public BookIdxView
{
    Q_OBJECT

public:
    explicit CurvesView(QWidget *parent = 0);
    ~CurvesView();

public:
    virtual void setCurrentCurveRunID(int runID);

protected:
    virtual void paintEvent(QPaintEvent * event);
    virtual QSize minimumSizeHint() const;
    virtual QSize sizeHint() const;
    virtual void mousePressEvent(QMouseEvent* event);
    virtual void mouseReleaseEvent(QMouseEvent* event);
    virtual void mouseMoveEvent(QMouseEvent* event);
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void currentChanged(const QModelIndex& current,
                                const QModelIndex& previous);
    virtual void resizeEvent(QResizeEvent *event);


private:
    QPainterPath* _errorPath;

    QPainterPath _sinPath();
    QPainterPath _stepPath();

    QList<TimeAndIndex*> _markers;

    void _paintCoplot(const QTransform& T, QPainter& painter,QPen& pen);
    void _paintErrorplot(const QTransform& T,
                         QPainter& painter, const QPen &pen,
                         const QModelIndex &plotIdx);
    void _paintCurve(const QModelIndex& curveIdx,
                     const QTransform &T, QPainter& painter,
                     bool isHighlight);
    void _paintMarkers(QPainter& painter);

    QModelIndex _chooseCurveNearMousePoint(const QPoint& pt);
    bool _isErrorCurveNearMousePoint(const QPoint& pt);

    QPixmap* _pixmap;
    QRectF _lastM;
    bool _isMeasure;
    QPoint _mouseCurrPos;
    QPixmap* _createLivePixmap();

    QString _format(double d);

    int _idxAtTimeBinarySearch(QPainterPath* path,
                               int low, int high, double time);

    // Key Events
    bool _isLastPoint;
    QPointF _lastPoint;
    void _keyPressSpace();
    void _keyPressUp();
    void _keyPressDown();
    void _keyPressPeriod();
    void _keyPressArrow(const Qt::ArrowType& arrow);
    void _keyPressComma();
    void _keyPressEscape();

protected slots:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsInserted(const QModelIndex &pidx, int start, int end);


};

#endif // CURVESVIEW_H

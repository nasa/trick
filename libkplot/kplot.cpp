#include "kplot.h"
#include <math.h>

KPlot::KPlot(KPlotModel* plotModel, QWidget *parent) :
    QWidget(parent),
    _plotModel(plotModel),
    _ptSize(0)
{
}

void KPlot::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);
    _paintSinTest();
    //_paintSinTestPattern();
}

QSize KPlot::minimumSizeHint() const
{
    QSize s(50,50);
    return s;
}

QSize KPlot::sizeHint() const
{
    QSize s(200,200);
    return s;
}


QPainterPath KPlot::_sinPath()
{
    // Make sin(t) t=[0,2*pi]
    QPainterPath path;
    path.moveTo(0,0);
    double t = 0;
    double dt = 0.01;
    while (1) {
        t += dt;
        if ( t > 2*M_PI-0.01 ) {
            t = 2*M_PI;
        }
        path.lineTo(t,sin(t));
        if ( t > 2*M_PI-0.01 ) break;
    }
    return path;
}

void KPlot::_paintSinTest()
{
    if ( !_plotModel ) return;

    QPainter painter(this);

    if ( _plotModel->curvePointSize(this) > 0 ) {
        painter.setRenderHint(QPainter::Antialiasing);
    }

    QTransform T = _plotModel->coordToPixelTransform(this);
    double ptSizeCurve = _plotModel->curvePointSize(this);
    QColor curveBlue(32,112,172);
    QPen pen(curveBlue);
    pen.setWidthF(ptSizeCurve);
    QPainterPath path = _sinPath();

    // Draw!
    painter.save();
    painter.setTransform(T);
    painter.setPen(pen);
    painter.drawPath(path);
    painter.restore();
}

void KPlot::_paintSinTestPattern()
{
    if ( !_plotModel ) return;

    QPainter painter(this);

    QTransform T = _plotModel->coordToPixelTransform(this);

    double ptSizeHLine = _plotModel->hLinePointSize(this);
    double ptSizeVLine = _plotModel->vLinePointSize(this);
    double ptSizeCurve = _plotModel->curvePointSize(this);

    //
    // Turn on antialiasing (works better with thin lines)
    //
    //painter.setRenderHint(QPainter::Antialiasing);

    //
    // Create diff pens for horiz line, vert line and curves
    //
    QPen hPen(Qt::black);
    hPen.setWidthF(ptSizeHLine);
    QPen vPen(Qt::black);
    vPen.setWidthF(ptSizeVLine);
    QPen cPen(Qt::red);
    cPen.setWidthF(ptSizeCurve);
    QPen piPen(Qt::red);
    piPen.setWidthF(ptSizeVLine);
    QPen dPen(Qt::cyan);
    dPen.setWidthF(ptSizeCurve);

    //
    // Create outter box, diagonals and cross
    //
    QVector<QPointF> a,b,c,d,e,f, xaxis, ypi, hlines, vlines, dlines;
    a << QPointF(0,1)       << QPointF(2*M_PI,1);
    b << QPointF(2*M_PI,1)  << QPointF(2*M_PI,-1);
    c << QPointF(2*M_PI,-1) << QPointF(0,-1);
    d << QPointF(0,-1)      << QPointF(0,1);
    e << QPointF(0,-1)      << QPointF(2*M_PI,1);
    f << QPointF(0,1)       << QPointF(2*M_PI,-1);
    xaxis << QPointF(0,0)   << QPointF(2*M_PI,0);
    ypi << QPointF(M_PI,-1) << QPointF(M_PI,1);
    hlines << a << c << xaxis ;
    vlines << b << d << ypi;
    dlines << e << f ;

    // Make x = pi axis
    QPainterPath piPath;
    piPath.moveTo(M_PI,-1);
    piPath.lineTo(M_PI,1);

    // Sin path
    QPainterPath path = _sinPath();

    //
    // Draw!
    //
    painter.save();
    painter.setTransform(T);
    painter.setPen(hPen);
    painter.drawLines(hlines);
    painter.setPen(vPen);
    painter.drawLines(vlines);
    painter.setPen(dPen);
    painter.drawLines(dlines);
    painter.setPen(cPen);
    painter.drawPath(path);
    painter.setPen(piPen);
    painter.drawPath(piPath);
    painter.restore();
}

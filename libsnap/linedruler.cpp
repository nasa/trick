#include "linedruler.h"
#include <QDebug>
#include <QPen>  // TODO: Delete this testing stuff
#include <QPainterPath>  // TODO: Delete this testing stuff
#include <math.h>

LinedRuler::LinedRuler(KPlotModel *plotModel,
                       Qt::Alignment alignment,
                       QWidget *parent) :
    QWidget(parent),
    _plotModel(plotModel),
    _alignment(alignment),
    _ptSize(0)
{
}

void LinedRuler::paintEvent(QPaintEvent* event)
{
    Q_UNUSED(event);

    if ( !_plotModel ) return;

    QPainter painter(this);

    QTransform T = _plotModel->coordToPixelTransform(this);

    double ptSizeHLine = _plotModel->hLinePointSize(this);
    double ptSizeVLine = _plotModel->vLinePointSize(this);

    QPen hPen(Qt::black);
    hPen.setWidthF(ptSizeHLine);
    QPen vPen(Qt::black);
    vPen.setWidthF(ptSizeVLine);

    QVector<QPointF> axis;
    QVector<QPointF> majorTics;
    QVector<QPointF> minorTics;
    QList<double> majors;
    QList<double> minors;

    const QRectF rect = _plotModel->mathRect(this);

    if ( _alignment == Qt::AlignTop ) {
        majors = _plotModel->majorXTics();
        minors = _plotModel->minorXTics();
        axis << QPointF(rect.left(),rect.top())
             << QPointF(rect.right(),rect.top());
        foreach ( double m, majors ) {
            majorTics << QPointF(m,rect.top()) << QPointF(m,rect.bottom());
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(m,rect.top()) << QPointF(m,rect.center().y());
        }
    } else if ( _alignment == Qt::AlignBottom ) {
        majors = _plotModel->majorXTics();
        minors = _plotModel->minorXTics();
        axis << QPointF(rect.left(),rect.bottom())
             << QPointF(rect.right(),rect.bottom());
        foreach ( double m, majors ) {
            majorTics << QPointF(m,rect.bottom()) << QPointF(m,rect.top());
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(m,rect.bottom()) << QPointF(m,rect.center().y());
        }
    } else if ( _alignment == Qt::AlignLeft ) {
        majors = _plotModel->majorYTics();
        minors = _plotModel->minorYTics();
        axis << QPointF(rect.left(),rect.top())
             << QPointF(rect.left(),rect.bottom());
        foreach ( double m, majors ) {
            majorTics << QPointF(rect.left(),m) << QPointF(rect.right(),m);
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(rect.left(),m) << QPointF(rect.center().x(),m);
        }
    } else if ( _alignment == Qt::AlignRight ) {
        majors = _plotModel->majorYTics();
        minors = _plotModel->minorYTics();
        axis << QPointF(rect.right(),rect.top())
             << QPointF(rect.right(),rect.bottom());
        foreach ( double m, majors ) {
            majorTics << QPointF(rect.right(),m) << QPointF(rect.left(),m);
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(rect.right(),m) << QPointF(rect.center().x(),m);
        }
    }

    //
    // Draw!
    //
    painter.save();
    painter.setTransform(T);
    if ( _alignment == Qt::AlignTop ||
         _alignment == Qt::AlignBottom ) {
        painter.setPen(hPen);
        painter.drawLines(axis);
        painter.setPen(vPen);
    } else {
        painter.setPen(vPen);
        painter.drawLines(axis);
        painter.setPen(hPen);
    }
    painter.drawLines(majorTics);
    painter.drawLines(minorTics);
    painter.restore();
}

QSize LinedRuler::minimumSizeHint() const
{
    QSize s(0,0);
    if ( _alignment == Qt::AlignTop ||
         _alignment == Qt::AlignBottom ) {
        s.setWidth(50);
        s.setHeight(8+_ptSize);  // ptSize for axis
    } else {
        s.setWidth(8+_ptSize);  // ptSize for axis
        s.setHeight(50);
    }
    return s;
}

QSize LinedRuler::sizeHint() const
{
    QSize s(0,0);
    if ( _alignment == Qt::AlignTop ||
         _alignment == Qt::AlignBottom ) {
        s.setWidth(50);
        s.setHeight(8+_ptSize);  // ptSize for axis
    } else {
        s.setWidth(8+_ptSize);  // ptSize for axis
        s.setHeight(50);
    }
    return s;
}

TicMark::TicMark(double value, const QSizeF &size)
{

}

void TicMark::draw(QPainter *painter)
{
}

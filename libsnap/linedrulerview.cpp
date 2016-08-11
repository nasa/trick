#include "linedrulerview.h"

LinedRulerView::LinedRulerView(Qt::Alignment alignment, QWidget *parent) :
    BookIdxView(parent),
    _alignment(alignment)
{
    setFrameShape(QFrame::NoFrame);
}

void LinedRulerView::_update()
{
    if ( model() ) {
        disconnect(model(),SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this,SLOT(rowsInserted(QModelIndex,int,int)));
    }
}

void LinedRulerView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    double ptSizeHLine = _hLinePointSize();
    double ptSizeVLine = _vLinePointSize();

    QPen hPen(Qt::black);
    hPen.setWidthF(ptSizeHLine);
    QPen vPen(Qt::black);
    vPen.setWidthF(ptSizeVLine);

    QVector<QPointF> axis;
    QVector<QPointF> majorTics;
    QVector<QPointF> minorTics;
    QList<double> majors;
    QList<double> minors;

    const QRectF M = _mathRect();

    QModelIndex plotIdx = _bookModel()->getIndex(_myIdx,"Plot");

    if ( _alignment == Qt::AlignTop ) {
        majors = _majorXTics(plotIdx);
        minors = _minorXTics();
        axis << QPointF(M.left(),M.top())
             << QPointF(M.right(),M.top())
             << QPointF(M.left(),M.bottom())
             << QPointF(M.right(),M.bottom());
        foreach ( double m, majors ) {
            majorTics << QPointF(m,M.top()) << QPointF(m,M.bottom());
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(m,M.top()) << QPointF(m,M.center().y());
        }
    } else if ( _alignment == Qt::AlignBottom ) {
        majors = _majorXTics(plotIdx);
        minors = _minorXTics();
        axis << QPointF(M.left(),M.bottom())
             << QPointF(M.right(),M.bottom())
             << QPointF(M.left(),M.top())
             << QPointF(M.right(),M.top());
        foreach ( double m, majors ) {
            majorTics << QPointF(m,M.bottom()) << QPointF(m,M.top());
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(m,M.bottom()) << QPointF(m,M.center().y());
        }
    } else if ( _alignment == Qt::AlignLeft ) {
        majors = _majorYTics(plotIdx);
        minors = _minorYTics();
        axis << QPointF(M.left(),M.top())
             << QPointF(M.left(),M.bottom())
             << QPointF(M.right(),M.top())
             << QPointF(M.right(),M.bottom());
        foreach ( double m, majors ) {
            majorTics << QPointF(M.left(),m) << QPointF(M.right(),m);
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(M.left(),m) << QPointF(M.center().x(),m);
        }
    } else if ( _alignment == Qt::AlignRight ) {
        majors = _majorYTics(plotIdx);
        minors = _minorYTics();
        axis << QPointF(M.right(),M.top())
             << QPointF(M.right(),M.bottom())
             << QPointF(M.left(),M.top())
             << QPointF(M.left(),M.bottom());
        foreach ( double m, majors ) {
            majorTics << QPointF(M.right(),m) << QPointF(M.left(),m);
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(M.right(),m) << QPointF(M.center().x(),m);
        }
    }

    //
    // Draw!
    //
    QPainter painter(viewport());
    painter.save();
    painter.setTransform(_coordToPixelTransform());
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

QSize LinedRulerView::minimumSizeHint() const
{
    QSize s(0,0);
    if ( _alignment == Qt::AlignTop ||
         _alignment == Qt::AlignBottom ) {
        s.setWidth(50);
        s.setHeight(8+_pointSize());  // ptSize for axis
    } else {
        s.setWidth(8+_pointSize());  // ptSize for axis
        s.setHeight(50);
    }
    return s;
}

QSize LinedRulerView::sizeHint() const
{
    QSize s(0,0);
    if ( _alignment == Qt::AlignTop ||
         _alignment == Qt::AlignBottom ) {
        s.setWidth(50);
        s.setHeight(8+_pointSize());  // ptSize for axis
    } else {
        s.setWidth(8+_pointSize());  // ptSize for axis
        s.setHeight(50);
    }
    return s;
}

void LinedRulerView::dataChanged(const QModelIndex &topLeft,
                                 const QModelIndex &bottomRight)
{
    if ( topLeft.parent() != _myIdx ) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft != bottomRight ) return;

    if ( topLeft == _plotMathRectIdx(_myIdx) ) {
        viewport()->update();
    }
}

void LinedRulerView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

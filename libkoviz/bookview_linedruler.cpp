#include "bookview_linedruler.h"

LinedRulerView::LinedRulerView(Qt::Alignment alignment, QWidget *parent) :
    BookIdxView(parent),
    _alignment(alignment)
{
    setFrameShape(QFrame::NoFrame);
}

void LinedRulerView::setModel(QAbstractItemModel* model)
{
    QAbstractItemView::setModel(model);
    if ( model ) {
        disconnect(model,SIGNAL(rowsInserted(QModelIndex,int,int)),
                   this,SLOT(rowsInserted(QModelIndex,int,int)));
    }
}

void LinedRulerView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    if ( !model() ) return;

    QPainter painter(viewport());

    double ptSizeHLine = 0.0;
    double ptSizeVLine = 0.0;

    QPen hPen = painter.pen();
    hPen.setWidthF(ptSizeHLine);
    QPen vPen = painter.pen();
    vPen.setWidthF(ptSizeVLine);

    QVector<QPointF> axis;
    QVector<QPointF> majorTics;
    QVector<QPointF> minorTics;
    QList<double> majors;
    QList<double> minors;

    const QRectF M = _mathRect();

    // (x0,y0) *---------------------------* (x1,y0)
    //         |                           |
    //         |             V             |
    //         |                           |
    // (x0,y1) *---------------------------* (x1,y1)
    const QRect V = viewport()->rect();
    double x0 = V.left();
    double y0 = V.top();
    double x1 = V.right();
    double y1 = V.bottom();

    QModelIndex plotIdx = _bookModel()->getIndex(rootIndex(),"Plot");

    if ( _alignment == Qt::AlignTop ) {
        axis << QPointF(x0,y0) << QPointF(x1,y0);
        majors = _majorXTics(plotIdx);
        minors = _minorXTics();
        foreach ( double m, majors ) {
            majorTics << QPointF(m,M.top()) << QPointF(m,M.bottom());
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(m,M.top()) << QPointF(m,M.center().y());
        }
    } else if ( _alignment == Qt::AlignBottom ) {
        axis << QPointF(x0,y1) << QPointF(x1,y1);
        majors = _majorXTics(plotIdx);
        minors = _minorXTics();
        foreach ( double m, majors ) {
            majorTics << QPointF(m,M.bottom()) << QPointF(m,M.top());
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(m,M.bottom()) << QPointF(m,M.center().y());
        }
    } else if ( _alignment == Qt::AlignLeft ) {
        axis << QPointF(x0,y0) << QPointF(x0,y1);
        majors = _majorYTics(plotIdx);
        minors = _minorYTics();
        foreach ( double m, majors ) {
            majorTics << QPointF(M.left(),m) << QPointF(M.right(),m);
        }
        foreach ( double m, minors ) {
            minorTics << QPointF(M.left(),m) << QPointF(M.center().x(),m);
        }
    } else if ( _alignment == Qt::AlignRight ) {
        axis << QPointF(x1,y0) << QPointF(x1,y1);
        majors = _majorYTics(plotIdx);
        minors = _minorYTics();
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
    painter.save();
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
    painter.setTransform(_coordToPixelTransform());
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
        s.setHeight(8);  // TODO: ptSize for axis??? 8+_pointSize()
    } else {
        s.setWidth(8);  // TODO: ptSize for axis
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
        s.setHeight(8);  // TODO:??? ptSize for axis
    } else {
        s.setWidth(8);  // TODO:??? ptSize for axis
        s.setHeight(50);
    }
    return s;
}

void LinedRulerView::dataChanged(const QModelIndex &topLeft,
                                 const QModelIndex &bottomRight)
{
    if ( topLeft.parent() != rootIndex() ) return;
    if ( topLeft.column() != 1 ) return;
    if ( topLeft != bottomRight ) return;

    if ( topLeft == _plotMathRectIdx(rootIndex()) ) {
        viewport()->update();
    }
}

void LinedRulerView::rowsInserted(const QModelIndex &parent, int start, int end)
{
    Q_UNUSED(parent);
    Q_UNUSED(start);
    Q_UNUSED(end);
}

#include "layoutitem_plottics.h"

PlotTicsLayoutItem::PlotTicsLayoutItem(const QFontMetrics &fontMetrics,
                                       PlotBookModel* bookModel,
                                       const QModelIndex& plotIdx) :
    PaintableLayoutItem(),
    _fontMetrics(fontMetrics),
    _bookModel(bookModel),
    _plotIdx(plotIdx),
    _cornerLayoutItem(fontMetrics,Qt::TopLeftCorner)
{
}

PlotTicsLayoutItem::~PlotTicsLayoutItem()
{
}

Qt::Orientations PlotTicsLayoutItem::expandingDirections() const
{
    Qt::Orientations orientations;
    if ( alignment() == Qt::AlignTop || alignment() == Qt::AlignBottom ) {
        orientations = Qt::Horizontal;
    } else if ( alignment() == Qt::AlignLeft || alignment() == Qt::AlignRight ){
        orientations = Qt::Vertical;
    }
    return orientations;
}

QRect PlotTicsLayoutItem::geometry() const
{
    return _rect;
}

bool PlotTicsLayoutItem::isEmpty() const
{
    return true;
}

QSize PlotTicsLayoutItem::maximumSize() const
{
    return QSize(0,0);
}

QSize PlotTicsLayoutItem::minimumSize() const
{
    return QSize(0,0);
}

void PlotTicsLayoutItem::setGeometry(const QRect &r)
{
    _rect = r;
}

QSize PlotTicsLayoutItem::sizeHint() const
{
    QSize size(0,0);
    if ( alignment() == Qt::AlignTop || alignment() == Qt::AlignBottom ) {
        size.setHeight(_cornerLayoutItem.sizeHint().height());
    } else if ( alignment() == Qt::AlignLeft || alignment() == Qt::AlignRight ){
        size.setWidth(_cornerLayoutItem.sizeHint().width());
    }
    return size;
}

void PlotTicsLayoutItem::paint(QPainter *painter,
                               const QRect &R, const QRect &RG,
                               const QRect &C, const QRectF &M)
{
    QVector<QPointF> axis;
    QVector<QPointF> majorTics;
    QVector<QPointF> minorTics;
    QList<double> majors;
    QList<double> minors;

    // (x0,y0) *---------------------------* (x1,y0)
    //         |                           |
    //         |             R             |
    //         |                           |
    // (x0,y1) *---------------------------* (x1,y1)
    double x0 = R.left();
    double y0 = R.top();
    double x1 = R.right();
    double y1 = R.bottom();

    const QRectF RM = mathRect(RG,C,M);
    QTransform T = coordToDotTransform(R,RM);

    if ( alignment() == Qt::AlignTop ) {
        axis << QPointF(x0,y0) << QPointF(x1,y0);

        majors = _bookModel->majorXTics(_plotIdx);
        minors = _bookModel->minorXTics(_plotIdx);
        foreach ( double m, majors ) {
            majorTics << T.map(QPointF(m,RM.top()))
                      << T.map(QPointF(m,RM.bottom()));
        }
        foreach ( double m, minors ) {
            minorTics << T.map(QPointF(m,RM.top()))
                      << T.map(QPointF(m,RM.center().y()));
        }
    } else if ( alignment() == Qt::AlignBottom ) {
        axis << QPointF(x0,y1) << QPointF(x1,y1);
        majors = _bookModel->majorXTics(_plotIdx);
        minors = _bookModel->minorXTics(_plotIdx);
        foreach ( double m, majors ) {
            double x = T.map(QPointF(m,1)).x();
            majorTics << QPointF(x,R.bottom())
                      << QPointF(x,R.top());
        }
        foreach ( double m, minors ) {
            double x = T.map(QPointF(m,1)).x();
            minorTics << QPointF(x,R.bottom())
                      << QPointF(x,R.center().y());
        }
    } else if ( alignment() == Qt::AlignLeft ) {
        axis << QPointF(x0,y0) << QPointF(x0,y1);
        majors = _bookModel->majorYTics(_plotIdx);
        minors = _bookModel->minorYTics(_plotIdx);
        foreach ( double m, majors ) {
            majorTics << T.map(QPointF(RM.left(),m))
                      << T.map(QPointF(RM.right(),m));
        }
        foreach ( double m, minors ) {
            minorTics << T.map(QPointF(RM.left(),m))
                      << T.map(QPointF(RM.center().x(),m));
        }
    } else if ( alignment() == Qt::AlignRight ) {
        axis << QPointF(x1,y0) << QPointF(x1,y1);
        majors = _bookModel->majorYTics(_plotIdx);
        minors = _bookModel->minorYTics(_plotIdx);
        foreach ( double m, majors ) {
            double y = T.map(QPointF(1,m)).y();
            majorTics << QPointF(R.right(),y)
                      << QPointF(R.left(),y);
        }
        foreach ( double m, minors ) {
            double y = T.map(QPointF(1,m)).y();
            minorTics << QPointF(R.right(),y)
                      << QPointF(R.center().x(),y);
        }
    }

    //
    // Draw!
    //
    QPen penOrig = painter->pen();
    QPen pen = painter->pen();
    int penWidth = painter->fontMetrics().xHeight()/7;  // 7 arbitrary
    pen.setWidth(penWidth);
    painter->setPen(pen);
    painter->drawLines(axis);
    painter->drawLines(majorTics);
    painter->drawLines(minorTics);
    painter->setPen(penOrig);
}

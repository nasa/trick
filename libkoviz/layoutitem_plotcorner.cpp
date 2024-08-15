#include "layoutitem_plotcorner.h"

PlotCornerLayoutItem::PlotCornerLayoutItem(const QFontMetrics &fontMetrics,
                                           Qt::Corner corner) :
    _fontMetrics(fontMetrics),
    _corner(corner)
{
}

PlotCornerLayoutItem::~PlotCornerLayoutItem()
{
}

Qt::Orientations PlotCornerLayoutItem::expandingDirections() const
{
    return Qt::Orientations();
}

QRect PlotCornerLayoutItem::geometry() const
{
    return _rect;
}

bool PlotCornerLayoutItem::isEmpty() const
{
    return true;
}

QSize PlotCornerLayoutItem::maximumSize() const
{
    return QSize(0,0);
}

QSize PlotCornerLayoutItem::minimumSize() const
{
    return QSize(0,0);
}

void PlotCornerLayoutItem::setGeometry(const QRect &r)
{
    _rect = r;
}

QSize PlotCornerLayoutItem::sizeHint() const
{
    QSize size;
    int s = _fontMetrics.xHeight()*4/3;
    size.setWidth(s);
    size.setHeight(s);
    return size;
}

void PlotCornerLayoutItem::paint(QPainter *painter,
                                 const QRect &R, const QRect &RG,
                                 const QRect &C, const QRectF &M)
{
    Q_UNUSED(RG);
    Q_UNUSED(C);
    Q_UNUSED(M);

    // (x0,y0) *--------------* (x1,y0)
    //         |              |
    //         |              |
    //         |              |
    // (x0,y1) *--------------* (x1,y1)

    double x0 = R.left();
    double y0 = R.top();
    double x1 = R.right();
    double y1 = R.bottom();

    QVector<QPointF> hline;
    QVector<QPointF> vline;

    if ( _corner == Qt::TopLeftCorner ) {
        hline << QPointF(x0,y0) << QPointF(x1,y0);
        vline << QPointF(x0,y0) << QPointF(x0,y1);
    } else if ( _corner == Qt::TopRightCorner ) {
        hline << QPointF(x1,y0) << QPointF(x0,y0);
        vline << QPointF(x1,y0) << QPointF(x1,y1);
    } else if ( _corner == Qt::BottomRightCorner ) {
        hline << QPointF(x1,y1) << QPointF(x0,y1);
        vline << QPointF(x1,y1) << QPointF(x1,y0);
    } else if ( _corner == Qt::BottomLeftCorner ) {
        hline << QPointF(x0,y1) << QPointF(x1,y1);
        vline << QPointF(x0,y1) << QPointF(x0,y0);
    } else {
        // Bad scoobs
    }

    //
    // Draw!
    //
    QPen penOrig = painter->pen();
    QPen pen = painter->pen();
    int penWidth = painter->fontMetrics().xHeight()/7;  // 7 arbitrary
    pen.setWidth(penWidth);
    painter->setPen(pen);
    painter->drawLines(hline);
    painter->drawLines(vline);
    painter->setPen(penOrig);
}
